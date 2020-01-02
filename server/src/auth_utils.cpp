#include <foxtrot/server/auth_utils.h>
#include <boost/filesystem.hpp>
#include <foxtrot/Logging.h>
#include <boost/property_tree/json_parser.hpp>

using namespace foxtrot;
namespace pt = boost::property_tree;

std::vector<unsigned char> detail::base642bin(const std::string& base64str)
{
    const int bin_maxlen = 4096;
    std::vector<unsigned char> out;
    //NOTE: max length!
    out.resize(bin_maxlen);
    size_t outlen;
    
    auto ret = sodium_base642bin(out.data(),bin_maxlen,
        base64str.data(), base64str.size(), nullptr, &outlen, nullptr,
                                 sodium_base64_VARIANT_ORIGINAL_NO_PADDING);
    
    if(ret)
        throw std::runtime_error("couldn't decode base64 string!");
    
    out.resize(outlen);
    return out;
}


std::array<unsigned char, CHALLENGE_STRING_BYTES> detail::get_challenge_bytes()
{
    std::array<unsigned char, CHALLENGE_STRING_BYTES> out;
    
    randombytes_buf(out.data(), out.size());
    return out;
}




std::tuple<pkarr, skarr> foxtrot::generate_new_key()
{
  pkarr pkout;
  skarr skout;
  
  //TODO: should we be checking return value of this?
  crypto_sign_keypair(pkout.data(), skout.data());
  
  return std::make_tuple(pkout,skout);
};

pt::ptree read_or_create_users_ptree(const std::string& fname, foxtrot::Logging& lg)
{
    pt::ptree creds_tree;
    if(!boost::filesystem::exists(fname))
    {
        auto path = boost::filesystem::path(fname);
        lg.strm(sl::info) << "requested credentials file: " << fname << " doesn't exist, creating it";
        if(!boost::filesystem::exists(path.parent_path()))
        {
            boost::filesystem::create_directories(path.parent_path());
        }
        boost::filesystem::ofstream ofs(fname);
        
        pt::write_json(ofs, creds_tree);
        
    }
    else
    {
        lg.strm(sl::info) << "reading existing credentials file: " << fname;
        //read existing credentials
        boost::filesystem::ifstream ifs(fname);
        pt::read_json(ifs, creds_tree);
    }
    
    return creds_tree;
};



void foxtrot::save_creds_to_file(const std::string& fname, const std::string& userid, const foxtrot::keypair& keys)
{
    foxtrot::Logging lg("save_creds_to_file");
    auto creds_tree = read_or_create_users_ptree(fname, lg);
    
    auto refkey =  userid;
    auto existing_value = creds_tree.get_optional<std::string>(refkey + ".secretkey");
    if(!existing_value)
    {
        lg.strm(sl::info) << "user secret key doesn't exist, creating new entry";
        auto pubkeystr = detail::bin2base64(std::get<0>(keys));
        auto secretkeystr = detail::bin2base64(std::get<1>(keys));
        creds_tree.put<std::string>(refkey + ".publickey",pubkeystr);
        creds_tree.put<std::string>(refkey + ".secretkey",secretkeystr);
        boost::filesystem::ofstream ofs(fname);
        pt::write_json(ofs, creds_tree);
    }
    else
    {
        lg.strm(sl::error) << "user already exists, refusing to create!";
    }

}

foxtrot::keypair foxtrot::get_creds_from_client_file(const std::string& fname, const std::string& userid)
{
    foxtrot::Logging lg("get_creds_from_file");
    if(!boost::filesystem::exists(fname))
    {
        lg.strm(sl::error) << "credentials file: " << fname << "doesn't exist, can't open";
        throw std::out_of_range("file doesn't exist");
    }
    
    pt::ptree creds_tree;
    boost::filesystem::ifstream ifs(fname);
    pt::read_json(ifs, creds_tree);

    auto refkey =  userid;
    auto seckeyb64 = creds_tree.get_optional<std::string>(refkey + ".secretkey");
    if(!seckeyb64)
    {
        lg.strm(sl::info) << "no secret key for userid: " << userid;
    }

    auto pubkeyb64 = creds_tree.get_optional<std::string>(refkey + ".publickey");
    if(!pubkeyb64)
    {
        //TODO: implement!!
        lg.strm(sl::info) << "no public key for userid: " << userid << ", generating from secret key";
    }
    
    auto pubkeyvec = detail::base642bin(*pubkeyb64);
    pkarr pubkey;
    std::move(pubkeyvec.begin(), pubkeyvec.begin() + pubkey.size(), pubkey.begin());

    skarr seckey;
    if(seckeyb64)
    {
        auto seckeyvec = detail::base642bin(*seckeyb64);
        std::move(seckeyvec.begin(), seckeyvec.begin() + seckey.size(), seckey.begin());
    }
    
    return std::make_tuple(pubkey, seckey);
    
}

void foxtrot::auth_user_to_file(const std::string& fname, const std::string& userid, const foxtrot::pkarr& pk, int authlevel, const std::string& keyname)
{
    foxtrot::Logging lg("auth_user_to_file");
    
    auto creds_tree = read_or_create_users_ptree(fname, lg);

    auto usertreeit = creds_tree.find(userid);
    if(usertreeit == creds_tree.not_found())
    {
        lg.strm(sl::info) << "user not already in server creds, adding it";
        creds_tree.put(userid + ".keys." + keyname, detail::bin2base64(pk));
        creds_tree.put(userid + ".authlevel", authlevel);
        boost::filesystem::ofstream ofs(fname);
        pt::write_json(ofs, creds_tree);
    }
    else
    {
        auto usertree = creds_tree.get_child(userid);
        auto keytree = usertree.get_child("keys");
        lg.strm(sl::info) << "found user: " << userid;
        auto keysit = keytree.find(keyname);
        if(keysit == keytree.not_found())
        {
            lg.strm(sl::info) << "no existing key with name: " << keyname << ". Adding new key to user";
            creds_tree.put(userid+ ".keys." + keyname, detail::bin2base64(pk));
        }
        else
        {
            auto currentkeystr = keysit->second.get_value<std::string>();
            auto keystr = detail::bin2base64(pk);
            if(currentkeystr != keystr)
            {
                lg.strm(sl::info) << "replacing existing public key with name: " << keyname;
                lg.strm(sl::info) << "existing key: " << currentkeystr;
                lg.strm(sl::info) << "new key: " << keystr;
                creds_tree.put(userid + ".keys." + keyname, keystr);
            }
        }

        auto current_authlevel = creds_tree.get<int>(userid + ".authlevel");
        if(current_authlevel != authlevel)
        {
            lg.strm(sl::info) << "changing authlevel from: " << current_authlevel << " to: " << authlevel;
            creds_tree.put<int>(userid + ".authlevel", authlevel);
        }
        
        boost::filesystem::ofstream ofs(fname);
        pt::write_json(ofs, creds_tree);
        
    }
}


void foxtrot::export_pubkey(const std::string& outfname, const std::string& userid, const foxtrot::pkarr& pk, const std::string& keyname)
{
    foxtrot::Logging lg("export_pubkey");
    auto creds_tree = read_or_create_users_ptree(outfname, lg);
    auto pubkeystr = detail::bin2base64(pk);
    creds_tree.put<std::string>(userid + ".publickey",pubkeystr);
    boost::filesystem::ofstream ofs(outfname);
    pt::write_json(ofs, creds_tree);
    
}



