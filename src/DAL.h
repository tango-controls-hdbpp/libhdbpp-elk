#include <iostream>
#include <string>
#include <stdint.h>
#include <stdio.h>
#include "json/json.hpp"
#include "restclient-cpp/connection.h"
#include "restclient-cpp/restclient.h"
#include "DataObjects.h"
#include "Log.h"

#ifndef _HDBPP_DAL_H
#define _HDBPP_DAL_H

#define ELK_INDEX "archiving_with_maps"
#define ELK_TYPE "doc"


using namespace std;
using json = nlohmann::json;

namespace HDBPP
{
const int HTTP_STATUS_OK = 200;
const int HTTP_STATUS_CREATED = 201;


// Parameters for an attribute that can be cached. Mapped to the attribute name
// in a map below.
struct AttributeParams
{
    AttributeParams(string param_id, string param_data_type, unsigned int param_ttl)
        : id(param_id)
        , data_type(param_data_type)
        , ttl(param_ttl)
    {
    }
    string id;
    string data_type;
    unsigned int ttl;
};

class DAL
{
private:
    string elk_http_repo;
    json errors;
    
    // cache the attribute name to some of its often used data, i.e. ttl and id. This
    // saves it being looked up in the database everytime we request it
    map<string, AttributeParams> attribute_cache;

    bool InsertElastic(string index, string type, string in_json, string& out_id);

    bool UpdateElastic(string index, string type, string id, string update_json_doc);

    bool SearchElastic(string index, string type, string json_search, json& out_json);

    bool GetElasticById(string index, string type, string id, json& out_json);

public:
    void SetElkHttpRepo(const string& elk_http_repo);
    // void SetErrors(const json& errors);
    const string& GetElkHttpRepo() const;
    const json& GetErrors() const;

    bool GetAttributeConfiguration(AttributeConfiguration& p_attr_conf);

    bool SaveAttributeConfiguration(AttributeConfiguration& p_attr_conf);

    bool GetAttributeConfigurationHistory(AttributeConfigurationHistory& p_attr_conf_history);

    bool SaveAttributeConfigurationHistory(AttributeConfigurationHistory& p_attr_conf_history);

    bool SaveAttributeParameter(AttributeParameter& p_attr_param);

    bool SaveAttributeEventData(AttributeEventData& p_attr_event_data);
    
    bool SaveDocument(Document& p_doc);

    DAL(string pelk_http_repo);
    ~DAL();
};
};
#endif