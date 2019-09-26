#include "DAL.h"

namespace HDBPP
{
DAL::DAL(string prepo)
{
    elk_http_repo = prepo;
}

DAL::~DAL()
{
}

void DAL::SetElkHttpRepo(const string& elk_http_repo)
{
    this->elk_http_repo = elk_http_repo;
}

const string& DAL::GetElkHttpRepo() const
{
    return elk_http_repo;
}

const json& DAL::GetErrors() const
{
    return errors;
}

bool DAL::InsertElastic(string index, string type, string in_json, string& out_id)
{
    std::stringstream qurl;
    qurl << elk_http_repo << "/" << index << "/" << type << "/";
    
    RestClient::init();
    RestClient::Connection* conn = new RestClient::Connection(qurl.str());
    RestClient::HeaderFields headers;
    headers["Content-Type"] = "application/json";
    conn->SetHeaders(headers);
    RestClient::Response r = conn->post("", in_json);

    bool found = false;
    if (r.code == HTTP_STATUS_CREATED) {
        found = true;
        auto res = json::parse(r.body);
        out_id = res["_id"];
    } else {
        errors += { r.code, r.body };
    }

    RestClient::disable();
    delete conn;

    return found;
}

bool DAL::UpdateElastic(string index, string type, string id, string update_json_doc)
{
    std::stringstream qurl;
    qurl << elk_http_repo << "/" << index << "/" << type << "/" << id << "/_update";

    RestClient::init();
    RestClient::Connection* conn = new RestClient::Connection(qurl.str());
    RestClient::HeaderFields headers;
    headers["Content-Type"] = "application/json";
    conn->SetHeaders(headers);

    std::stringstream elk_json_update_in;
    elk_json_update_in << "{\"script\" : {"
                       << "\"source\": \"" << update_json_doc << "\","
                       << "\"lang\": \"painless\" }}";
    string jsonstr = elk_json_update_in.str();
    RestClient::Response r = conn->post("", jsonstr);

    bool found = false;
    if (r.code == HTTP_STATUS_OK)
        found = true;
    else {
        errors += { r.code, r.body };
    }

    RestClient::disable();
    delete conn;
    return found;
}

bool DAL::SearchElastic(string index, string type, string json_search, json& out_json)
{
    std::stringstream qurl;
    qurl << elk_http_repo << "/" << index << "/" << type << "/"
         << "_search";

    RestClient::init();
    RestClient::Connection* conn = new RestClient::Connection(qurl.str());
    RestClient::HeaderFields headers;
    headers["Content-Type"] = "application/json";
    conn->SetHeaders(headers);
    RestClient::Response r = conn->post("", json_search);

    bool found = false;
    if (r.code == HTTP_STATUS_OK) {
        // found = true;
        out_json = json::parse(r.body);
        if (out_json["hits"]["total"] > 0)
            found = true;
    } else {
        errors += { r.code, r.body };
    }

    RestClient::disable();
    delete conn;
    return found;
}

bool DAL::GetElasticById(string index, string type, string id, json& out_json)
{
    std::stringstream qurl;
    qurl << elk_http_repo << "/" << index << "/" << type << "/" << id;

    RestClient::init();
    RestClient::Connection* conn = new RestClient::Connection(qurl.str());
    RestClient::HeaderFields headers;
    headers["Content-Type"] = "application/json";
    conn->SetHeaders(headers);
    RestClient::Response r = conn->get("");

    bool found = false;
    if (r.code == HTTP_STATUS_OK) {
        found = true;
        out_json = json::parse(r.body);
    } else {
        errors += { r.code, r.body };
    }

    RestClient::disable();
    delete conn;
    return found;
}

bool DAL::GetAttributeConfiguration(AttributeConfiguration& p_attr_conf)
{
    try
    {
        map<string, AttributeParams>::iterator it = attribute_cache.find(p_attr_conf.GetJsonQuery());

        if (it != attribute_cache.end()) {
            LOG(Debug) << "Attribute configuration cache: " << it->second.id << endl;
            p_attr_conf.SetDataType(it->second.data_type);
            p_attr_conf.SetTtl(it->second.ttl);
            p_attr_conf.SetID(it->second.id);
            return true;
        }

        json res;
        if (!SearchElastic(ELK_INDEX, ELK_TYPE, p_attr_conf.GetJsonQuery(), res)){
            LOG(Debug) << "(Attribute configuration DB) No result for " << p_attr_conf.GetJsonQuery() << endl;
            return false;
            
        }

        LOG(Debug) << "Attribute configuration DB: " << res << endl;

        json the_one_we_search = res["hits"]["hits"][0]["_source"];

        p_attr_conf.SetParameterFromJson(the_one_we_search);
        p_attr_conf.SetID(res["hits"]["hits"][0]["_id"]);

        attribute_cache.insert(
            make_pair(p_attr_conf.GetJsonQuery(),
                      AttributeParams(p_attr_conf.GetID(), p_attr_conf.GetDataType(), p_attr_conf.GetTtl())));

        return true;
    }
    catch (int e)
    {
        LOG(Debug) << "Ex#" << e << "::" << errors.dump() << endl;
        return false;
    }
}

bool DAL::SaveAttributeConfiguration(AttributeConfiguration& p_attr_conf)
{
    try
    {
        json res;
        if (!p_attr_conf.GetID().empty()) {
            // update if not empty
            LOG(Debug) << "updating attribute configuration: " << p_attr_conf.GetID() << endl;
            return UpdateElastic(ELK_INDEX, ELK_TYPE, p_attr_conf.GetID(), p_attr_conf.ToElkScript4Update());
        }

        string out_id;
        if (!InsertElastic(ELK_INDEX, ELK_TYPE, p_attr_conf.ToJson(), out_id))
            return false;
        LOG(Debug) << "Inserted attribute configuration: " << out_id << endl;
        p_attr_conf.SetID(out_id);
        return true;
    }
    catch (int e)
    {
        LOG(Debug) << "Ex#" << e << "::" << errors.dump() << endl;
        return false;
    }
}

bool DAL::GetAttributeConfigurationHistory(AttributeConfigurationHistory& p_attr_conf_history)
{
    try
    {
        json res;
        if (!SearchElastic(ELK_INDEX, ELK_TYPE, p_attr_conf_history.GetJsonQuery(), res))
            return false;

        LOG(Debug) << "Attribute configuration history: \n" << res << endl;
        json the_one_we_search = res["hits"]["hits"][0]["_source"];

        p_attr_conf_history.SetParameterFromJson(the_one_we_search);
        p_attr_conf_history.SetID(res["hits"]["hits"][0]["_id"]);
        return true;
    }
    catch (int e)
    {
        LOG(Debug) << "Ex#" << e << "::" << errors.dump() << endl;
        return false;
    }
}

bool DAL::SaveAttributeConfigurationHistory(AttributeConfigurationHistory& p_attr_conf_history)
{
    try
    {
        json res;
        if (!p_attr_conf_history.GetID().empty()) {
            LOG(Debug) << "Updating attribute configuration history: " << p_attr_conf_history.GetID() << endl;
            return UpdateElastic(
                ELK_INDEX, ELK_TYPE, p_attr_conf_history.GetID(), p_attr_conf_history.ToElkScript4Update());
        }

        string out_id;
        if (!InsertElastic(ELK_INDEX, ELK_TYPE, p_attr_conf_history.ToJson(), out_id))
            return false;
        LOG(Debug) << "Inserted attribute configuration history: " << out_id << endl;
        p_attr_conf_history.SetID(out_id);
        return true;
    }
    catch (int e)
    {
        LOG(Debug) << "Ex#" << e << "::" << errors.dump() << endl;
        return false;
    }
}

bool DAL::SaveAttributeParameter(AttributeParameter& p_attr_param)
{
    try
    {
        json res;
        if (SearchElastic(ELK_INDEX, ELK_TYPE, p_attr_param.GetJsonQuery(), res))
        {
            p_attr_param.SetID(res["hits"]["hits"][0]["_id"]);
        }
        
        if (!p_attr_param.GetID().empty()) {
            LOG(Debug) << "Updating attribute parameter: " << p_attr_param.GetID() << endl;
            //LOG(Debug) << "ElkScript4Update: " << p_attr_param.ToElkScript4Update()<< endl;
            return UpdateElastic(ELK_INDEX, ELK_TYPE, p_attr_param.GetID(), p_attr_param.ToElkScript4Update());
        }

        string out_id;
        if (!InsertElastic(ELK_INDEX, ELK_TYPE, p_attr_param.ToJson(), out_id))
            return false;
        LOG(Debug) << "Inserted attribute parameter: " << out_id << endl;
        p_attr_param.SetID(out_id);
        return true;
    }
    catch (int e)
    {
        LOG(Debug) << "Ex#" << e << "::" << errors.dump() << endl;
        return false;
    }
}

bool DAL::SaveDocument(Document& p_doc)
{
    try
    {
        json res;
        if (!p_doc.GetID().empty()) {
            throw string("The data object 'AttributeEventData' should not be updated in the DB");
        }

        string out_id;
        if (!InsertElastic(ELK_INDEX, ELK_TYPE, p_doc.ToJson(), out_id))
            return false;
        LOG(Debug) << "Inserted Document: " << out_id << endl;
        p_doc.SetID(out_id);
        return true;
    }
    catch (int e)
    {
        LOG(Debug) << "Ex#" << e << "::" << errors.dump() << endl;
        return false;
    }
}

bool DAL::SaveAttributeEventData(AttributeEventData& p_attr_event_data)
{
    try
    {
        json res;
        if (!p_attr_event_data.GetID().empty()) {
            throw string("The data object 'AttributeEventData' should not be updated in the DB");
        }
        LOG(Debug) << "Inserting AttributeEventData: " << p_attr_event_data.ToJson() << endl;
        string out_id;
        if (!InsertElastic(ELK_INDEX, ELK_TYPE, p_attr_event_data.ToJson(), out_id))
            return false;
        LOG(Debug) << "Inserted AttributeEventData: " << out_id << endl;
        p_attr_event_data.SetID(out_id);
        return true;
    }
    catch (int e)
    {
        LOG(Debug) << "Ex#" << e << "::" << errors.dump() << endl;
        return false;
    }
}
};
