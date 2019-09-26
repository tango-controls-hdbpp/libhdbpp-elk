#include "DataObjects.h"

namespace HDBPP
{
void AttributeConfiguration::SetDataType(string str)
{
    data_type = str;
}

void AttributeConfiguration::SetDomain(string str)
{
    domain = str;
}

void AttributeConfiguration::SetFamily(string str)
{
    family = str;
}

void AttributeConfiguration::SetMember(string str)
{
    member = str;
}

void AttributeConfiguration::SetFacility(string str)
{
    facility = str;
}

void AttributeConfiguration::SetPort(int str)
{
    port = str;
}

void AttributeConfiguration::SetTtl(int str)
{
    ttl = str;
}

string AttributeConfiguration::GetDataType()
{
    return data_type;
}

string AttributeConfiguration::GetDomain()
{
    return domain;
}

string AttributeConfiguration::GetFamily()
{
    return family;
}

string AttributeConfiguration::GetMember()
{
    return member;
}

string AttributeConfiguration::GetFacility()
{
    return facility;
}

int AttributeConfiguration::GetPort()
{
    return port;
}

int AttributeConfiguration::GetTtl()
{
    return ttl;
}

void AttributeConfiguration::SetID(string ID)
{
    this->ID = ID;
}

string AttributeConfiguration::GetID()
{
    return ID;
}

string AttributeConfiguration::GetName()
{
    return name;
}

void AttributeConfiguration::SetName(string str)
{
    name = str;
}

AttributeConfiguration::AttributeConfiguration(string pdata_type,
                                               string pdomain,
                                               string pfamily,
                                               string pmember,
                                               string pname,
                                               string pfacility,
                                               int pport,
                                               int pttl)
{
    data_type = pdata_type;
    domain = pdomain;
    family = pfamily;
    member = pmember;
    name = pname;
    facility = pfacility;
    port = pport;
    ttl = pttl;
}

AttributeConfiguration::AttributeConfiguration(AttributeName& pattr_name)
{
    domain = pattr_name.domain();
    family = pattr_name.family();
    member = pattr_name.member();
    name = pattr_name.name();

    std::string s = pattr_name.tango_host();
    std::string delimiter = ":";
    int pos = s.find(delimiter);
    facility = s.substr(0, pos); // localhost
    s = s.substr(pos + delimiter.length());
    port = atoi(s.c_str());
}

AttributeConfiguration::AttributeConfiguration(json& p_json)
{
    SetParameterFromJson(p_json);
}

AttributeConfiguration::~AttributeConfiguration()
{
}

void AttributeConfiguration::SetParameterFromJson(json& p_json)
{
    data_type = p_json["data_type"];
    domain = p_json["domain"];
    family = p_json["family"];
    member = p_json["member"];
    name = p_json["name"];
    facility = p_json["facility"];
    port = p_json["port"];
    ttl = p_json["ttl"];
}

string AttributeConfiguration::GetJsonQuery()
{
    ostringstream json_str;
    json_str << "{"
             << "\"query\": {"
             << "\"bool\": {"
             << "\"must\": ["
             << " { \"match\": { \"facility\": \"" << facility << "\" } },"
             << " { \"match\": { \"port\": " << port << " } },"
             << " { \"match\": { \"domain\": \"" << domain << "\" } },"
             << " { \"match\": { \"family\": \"" << family << "\" } } , "
             << " { \"match\": { \"member\": \"" << member << "\" } },"
             << " { \"match\": { \"name\": \"" << name << "\" } },"
             << " { \"match\": { \"DataObject\": \"AttributeConfiguration\" } }"
             << "]"
             << "}"
             << "}, \"size\": 1"
             << "}";
    string result = json_str.str().c_str();
    return result;
}

string AttributeConfiguration::ToJson()
{
    json j2 = { { "data_type", data_type },
                { "domain", domain },
                { "family", family },
                { "member", member },
                { "name", name },
                { "facility", facility },
                { "port", port },
                { "ttl", ttl },
                { "DataObject", "AttributeConfiguration" } };

    return j2.dump();
}

string AttributeConfiguration::ToElkScript4Update()
{
    ostringstream json_str;
    json_str << "ctx._source.data_type = '" << data_type << "';"
             << "ctx._source.domain = '" << domain << "';"
             << "ctx._source.family = '" << family << "';"
             << "ctx._source.member = '" << member << "';"
             << "ctx._source.name = '" << name << "';"
             << "ctx._source.facility = '" << facility << "';"
             << "ctx._source.port = " << port << ";"
             << "ctx._source.ttl = " << ttl << ";";

    string result = json_str.str().c_str();
    return result;
}
};
void HDBPP::AttributeConfigurationHistory::SetAttributeConfigurationID(string str)
{
    attribute_configuration_id = str;
}

void HDBPP::AttributeConfigurationHistory::SetEventType(string str)
{
    eventType = str;
}

void HDBPP::AttributeConfigurationHistory::SetTime(int64_t ptime)
{
    time = ptime;
}

void HDBPP::AttributeConfigurationHistory::SetTimeUs(int ptime_us)
{
    this->time_us = ptime_us;
}

void HDBPP::AttributeConfigurationHistory::SetID(string id)
{
    this->ID = id;
}

string HDBPP::AttributeConfigurationHistory::GetAttributeConfigurationID()
{
    return attribute_configuration_id;
}

string HDBPP::AttributeConfigurationHistory::GetEventType()
{
    return eventType;
}

int64_t HDBPP::AttributeConfigurationHistory::GetTime()
{
    return time;
}

int HDBPP::AttributeConfigurationHistory::GetTimeUs()
{
    return time_us;
}

string HDBPP::AttributeConfigurationHistory::GetID()
{
    return ID;
}

HDBPP::AttributeConfigurationHistory::AttributeConfigurationHistory(AttributeConfiguration pattr_conf)
{
    attribute_configuration_id = pattr_conf.GetID();
}

HDBPP::AttributeConfigurationHistory::AttributeConfigurationHistory(AttributeConfiguration pattr_conf,
                                                                    string peventType,
                                                                    int64_t ptime,
                                                                    int ptime_us)
{
    attribute_configuration_id = pattr_conf.GetID();
    eventType = peventType;
    time = ptime;
    time_us = ptime_us;
}

HDBPP::AttributeConfigurationHistory::AttributeConfigurationHistory(string pattr_conf_id,
                                                                    string peventType,
                                                                    int64_t ptime,
                                                                    int ptime_us)
{
    attribute_configuration_id = pattr_conf_id;
    eventType = peventType;
    time = ptime;
    time_us = ptime_us;
}

HDBPP::AttributeConfigurationHistory::~AttributeConfigurationHistory()
{
}

string HDBPP::AttributeConfigurationHistory::ToJson()
{
    json j2 = { { "attribute_configuration_id", attribute_configuration_id },
                { "eventType", eventType },
                { "time", time },
                { "time_us", time_us },
                { "DataObject", "AttributeConfigurationHistory" } };

    return j2.dump();
}

string HDBPP::AttributeConfigurationHistory::ToElkScript4Update()
{
    ostringstream json_str;
    json_str << "ctx._source.attribute_configuration_id = '" << attribute_configuration_id << "';"
             << "ctx._source.eventType = '" << eventType << "';"
             << "ctx._source.time = " << time << "L;"
             << "ctx._source.time_us = " << time_us << "L;";

    string result = json_str.str().c_str();
    return result;
}

string HDBPP::AttributeConfigurationHistory::GetJsonQuery()
{
    ostringstream json_str;
    json_str << "{"
             << "\"query\": {"
             << "\"bool\": {"
             << "\"must\": ["
             << " { \"match\": { \"attribute_configuration_id\": \"" << attribute_configuration_id << "\" } },"
             << " { \"match\": { \"DataObject\": \"AttributeConfigurationHistory\" } }"
             << "]"
             << "}"
             << "}, \"size\": 1"
             << "}";
    string result = json_str.str().c_str();
    return result;
}

void HDBPP::AttributeConfigurationHistory::SetParameterFromJson(json& p_json)
{
    attribute_configuration_id = p_json["attribute_configuration_id"];
    eventType = p_json["eventType"];
    try
    {
        time = p_json["time"];
    }
    catch (int e)
    {
        string str_time = p_json["time"];
        std::istringstream iss(str_time);
        iss >> time;
    }
    time_us = p_json["time_us"];
}

HDBPP::AttributeConfigurationHistory::AttributeConfigurationHistory(json& pjson)
{
    SetParameterFromJson(pjson);
}

void HDBPP::AttributeParameter::SetID(const string& ID)
{
    this->ID = ID;
}
void HDBPP::AttributeParameter::SetArchiveAbsChange(const string& archiveAbsChange)
{
    this->archiveAbsChange = archiveAbsChange;
}
void HDBPP::AttributeParameter::SetArchivePeriod(const string& archivePeriod)
{
    this->archivePeriod = archivePeriod;
}
void HDBPP::AttributeParameter::SetArchiveRelChange(const string& archiveRelChange)
{
    this->archiveRelChange = archiveRelChange;
}
void HDBPP::AttributeParameter::SetDescription(const string& description)
{
    this->description = description;
}
void HDBPP::AttributeParameter::SetDiplayUnit(const string& diplayUnit)
{
    this->diplayUnit = diplayUnit;
}
void HDBPP::AttributeParameter::SetEvTime(const int64_t& ev_time)
{
    this->ev_time = ev_time;
}
void HDBPP::AttributeParameter::SetEvTimeUs(int ev_time_us)
{
    this->ev_time_us = ev_time_us;
}
void HDBPP::AttributeParameter::SetFormat(const string& format)
{
    this->format = format;
}
void HDBPP::AttributeParameter::SetInsertTime(const int64_t& insert_time)
{
    this->insert_time = insert_time;
}
void HDBPP::AttributeParameter::SetInsertTimeUs(int insert_time_us)
{
    this->insert_time_us = insert_time_us;
}
void HDBPP::AttributeParameter::SetLabel(const string& label)
{
    this->label = label;
}
void HDBPP::AttributeParameter::SetStandardUnit(const string& standardUnit)
{
    this->standardUnit = standardUnit;
}
void HDBPP::AttributeParameter::SetUnit(const string& unit)
{
    this->unit = unit;
}
const string& HDBPP::AttributeParameter::GetID() const
{
    return ID;
}
const string& HDBPP::AttributeParameter::GetArchiveAbsChange() const
{
    return archiveAbsChange;
}
const string& HDBPP::AttributeParameter::GetArchivePeriod() const
{
    return archivePeriod;
}
const string& HDBPP::AttributeParameter::GetArchiveRelChange() const
{
    return archiveRelChange;
}
const string& HDBPP::AttributeParameter::GetDescription() const
{
    return description;
}
const string& HDBPP::AttributeParameter::GetDiplayUnit() const
{
    return diplayUnit;
}
const int64_t& HDBPP::AttributeParameter::GetEvTime() const
{
    return ev_time;
}
int HDBPP::AttributeParameter::GetEvTimeUs() const
{
    return ev_time_us;
}
const string& HDBPP::AttributeParameter::GetFormat() const
{
    return format;
}
const int64_t& HDBPP::AttributeParameter::GetInsertTime() const
{
    return insert_time;
}
int HDBPP::AttributeParameter::GetInsertTimeUs() const
{
    return insert_time_us;
}
const string& HDBPP::AttributeParameter::GetLabel() const
{
    return label;
}
const string& HDBPP::AttributeParameter::GetStandardUnit() const
{
    return standardUnit;
}
const string& HDBPP::AttributeParameter::GetUnit() const
{
    return unit;
}

void HDBPP::AttributeParameter::SetParameterFromJson(json& p_json)
{
    SetAttributeConfigurationID(p_json[PARAM_COL_ID]);
    SetEvTime(p_json[PARAM_COL_EV_TIME]);
    SetEvTimeUs(p_json[PARAM_COL_EV_TIME_US]);
    SetInsertTime(p_json[PARAM_COL_INS_TIME]);
    SetInsertTimeUs(p_json[PARAM_COL_INS_TIME_US]);
    SetLabel(p_json[PARAM_COL_LABEL]);
    SetUnit(p_json[PARAM_COL_UNIT]);
    SetStandardUnit(p_json[PARAM_COL_STANDARDUNIT]);
    SetFormat(p_json[PARAM_COL_FORMAT]);
    SetDiplayUnit(p_json[PARAM_COL_DISPLAYUNIT]);
    SetArchiveRelChange(p_json[PARAM_COL_ARCHIVERELCHANGE]);
    SetArchiveAbsChange(p_json[PARAM_COL_ARCHIVEABSCHANGE]);
    SetArchivePeriod(p_json[PARAM_COL_ARCHIVEPERIOD]);
    SetDescription(p_json[PARAM_COL_DESCRIPTION]);
}

string HDBPP::AttributeParameter::ToJson()
{
    json j = { { PARAM_COL_ID, GetAttributeConfigurationID() },
               { PARAM_COL_EV_TIME, GetEvTime() },
               { PARAM_COL_EV_TIME_US, GetEvTimeUs() },
               { PARAM_COL_INS_TIME, GetInsertTime() },
               { PARAM_COL_INS_TIME_US, GetInsertTimeUs() },
               { PARAM_COL_LABEL, GetLabel() },
               { PARAM_COL_UNIT, GetUnit() },
               { PARAM_COL_STANDARDUNIT, GetStandardUnit() },
               { PARAM_COL_DISPLAYUNIT, GetDiplayUnit() },
               { PARAM_COL_FORMAT, GetFormat() },
               { PARAM_COL_ARCHIVERELCHANGE, GetArchiveRelChange() },
               { PARAM_COL_ARCHIVEABSCHANGE, GetArchiveAbsChange() },
               { PARAM_COL_ARCHIVEPERIOD, GetArchivePeriod() },
               { PARAM_COL_DESCRIPTION, GetDescription() },
               { "DataObject", "AttributeParameter" } };

    return j.dump();
}

string HDBPP::AttributeParameter::ToElkScript4Update()
{
    ostringstream json_str;
    json_str << "ctx._source." << PARAM_COL_ID << " = '" << GetAttributeConfigurationID() << "';"
             << "ctx._source." << PARAM_COL_EV_TIME << " = '" << GetEvTime() << "';"
             << "ctx._source." << PARAM_COL_EV_TIME_US << " = '" << GetEvTimeUs() << "';"
             << "ctx._source." << PARAM_COL_INS_TIME << " = '" << GetInsertTime() << "';"
             << "ctx._source." << PARAM_COL_INS_TIME_US << " = '" << GetInsertTimeUs() << "';"
             << "ctx._source." << PARAM_COL_LABEL << " = '" << GetLabel() << "';"
             << "ctx._source." << PARAM_COL_UNIT << " = '" << GetUnit() << "';"
             << "ctx._source." << PARAM_COL_STANDARDUNIT << " = '" << GetStandardUnit() << "';"
             << "ctx._source." << PARAM_COL_DISPLAYUNIT << " = '" << GetDiplayUnit() << "';"
             << "ctx._source." << PARAM_COL_FORMAT << " = '" << GetFormat() << "';"
             << "ctx._source." << PARAM_COL_ARCHIVERELCHANGE << " = '" << GetArchiveRelChange() << "';"
             << "ctx._source." << PARAM_COL_ARCHIVEABSCHANGE << " = '" << GetArchiveAbsChange() << "';"
             << "ctx._source." << PARAM_COL_ARCHIVEPERIOD << " = '" << GetArchivePeriod() << "';"
             << "ctx._source." << PARAM_COL_DESCRIPTION << " = '" << GetDescription() << "';";

    string result = json_str.str().c_str();
    return result;
}

string HDBPP::AttributeParameter::GetJsonQuery()
{
    ostringstream json_str;
    json_str << "{"
             << "\"query\": {"
             << "\"bool\": {"
             << "\"must\": ["
             << " { \"match\": { \"" << PARAM_COL_ID << "\" : \"" << GetAttributeConfigurationID() << "\" } },"
             << " { \"match\": { \"DataObject\": \"AttributeParameter\" } }"
             << "]"
             << "}"
             << "}, \"size\": 1"
             << "}";
    string result = json_str.str().c_str();
    return result;
}

HDBPP::AttributeParameter::AttributeParameter(string pattribute_configuration_id,
                                              int64_t pev_time,
                                              int pev_time_us,
                                              int64_t pinsert_time,
                                              int pinsert_time_us,
                                              string plabel,
                                              string punit,
                                              string pstandardUnit,
                                              string pdiplayUnit,
                                              string pformat,
                                              string parchiveRelChange,
                                              string parchiveAbsChange,
                                              string parchivePeriod,
                                              string pdescription)
{
    attribute_configuration_id = pattribute_configuration_id;
    ev_time = pev_time;
    ev_time_us = pev_time_us;
    insert_time = pinsert_time;
    insert_time_us = pinsert_time_us;
    label = plabel;
    unit = punit;
    standardUnit = pstandardUnit;
    diplayUnit = pdiplayUnit;
    description = pdescription;
    format = pformat;
    archiveRelChange = parchiveRelChange;
    archiveAbsChange = parchiveAbsChange;
    archivePeriod = parchivePeriod;
}

HDBPP::AttributeParameter::~AttributeParameter()
{
}

void HDBPP::AttributeParameter::SetAttributeConfigurationID(const string& str)
{
    attribute_configuration_id = str;
}

const string& HDBPP::AttributeParameter::GetAttributeConfigurationID() const
{
    return attribute_configuration_id;
}

void HDBPP::AttributeEventData::SetAttributeConfigurationId(const string& attribute_configuration_id)
{
    this->attribute_configuration_id = attribute_configuration_id;
}
void HDBPP::AttributeEventData::SetErrorDesc(const string& errorDesc)
{
    this->errorDesc = errorDesc;
}
void HDBPP::AttributeEventData::SetEvTime(const int64_t& ev_time)
{
    this->ev_time = ev_time;
}
void HDBPP::AttributeEventData::SetEvTimeUs(int ev_time_us)
{
    this->ev_time_us = ev_time_us;
}
void HDBPP::AttributeEventData::SetInsertTime(const int64_t& insert_time)
{
    this->insert_time = insert_time;
}
void HDBPP::AttributeEventData::SetInsertTimeUs(int insert_time_us)
{
    this->insert_time_us = insert_time_us;
}
void HDBPP::AttributeEventData::SetPeriod(const string& period)
{
    this->period = period;
}
void HDBPP::AttributeEventData::SetQuality(int quality)
{
    this->quality = quality;
}
void HDBPP::AttributeEventData::SetRcvTime(const int64_t& rcv_time)
{
    this->rcv_time = rcv_time;
}
void HDBPP::AttributeEventData::SetRcvTimeUs(int rcv_time_us)
{
    this->rcv_time_us = rcv_time_us;
}
void HDBPP::AttributeEventData::SetTtl(int ttl)
{
    this->ttl = ttl;
}
void HDBPP::AttributeEventData::SetValueR(const json& value_r)
{
    this->value_r = value_r;
}
void HDBPP::AttributeEventData::SetValueW(const json& value_w)
{
    this->value_w = value_w;
}

const string& HDBPP::AttributeEventData::GetAttributeConfigurationId() const
{
    return attribute_configuration_id;
}
const string& HDBPP::AttributeEventData::GetErrorDesc() const
{
    return errorDesc;
}
const int64_t& HDBPP::AttributeEventData::GetEvTime() const
{
    return ev_time;
}
int HDBPP::AttributeEventData::GetEvTimeUs() const
{
    return ev_time_us;
}
const int64_t& HDBPP::AttributeEventData::GetInsertTime() const
{
    return insert_time;
}
int HDBPP::AttributeEventData::GetInsertTimeUs() const
{
    return insert_time_us;
}
const string& HDBPP::AttributeEventData::GetPeriod() const
{
    return period;
}
int HDBPP::AttributeEventData::GetQuality() const
{
    return quality;
}
const int64_t& HDBPP::AttributeEventData::GetRcvTime() const
{
    return rcv_time;
}
int HDBPP::AttributeEventData::GetRcvTimeUs() const
{
    return rcv_time_us;
}
int HDBPP::AttributeEventData::GetTtl() const
{
    return ttl;
}
const json& HDBPP::AttributeEventData::GetValueR() const
{
    return value_r;
}
const json& HDBPP::AttributeEventData::GetValueW() const
{
    return value_w;
}

void HDBPP::AttributeEventData::SetID(const string& pID)
{
    ID = pID;
}

const string& HDBPP::AttributeEventData::GetID() const
{
    return ID;
}

HDBPP::AttributeEventData::AttributeEventData(string pattribute_configuration_id,
                                              string pperiod,
                                              int64_t pev_time,
                                              int pev_time_us,
                                              int64_t prcv_time,
                                              int prcv_time_us,
                                              int64_t pinsert_time,
                                              int pinsert_time_us,
                                              int pquality,
                                              string perrorDesc,
                                              json& pvalue_r,
                                              json& pvalue_w,
                                              int pttl)
{
    attribute_configuration_id = pattribute_configuration_id;
    period = pperiod;
    ev_time = pev_time;
    ev_time_us = pev_time_us;
    rcv_time = prcv_time;
    rcv_time_us = prcv_time_us;
    insert_time = pinsert_time;
    insert_time_us = pinsert_time_us;
    quality = pquality;
    errorDesc = perrorDesc;
    value_r = pvalue_r;
    value_w = pvalue_w;
    ttl = pttl;
}

HDBPP::AttributeEventData::~AttributeEventData()
{
}

void HDBPP::AttributeEventData::SetParameterFromJson(json& p_json)
{
    throw string("The data object 'AttributeEventData' should not be read from the DB");
}

string HDBPP::AttributeEventData::ToJson()
{
    json j = { { SC_COL_ID, GetAttributeConfigurationId() },
               { SC_COL_PERIOD, GetPeriod() },
               { SC_COL_EV_TIME, GetEvTime() },
               { SC_COL_EV_TIME_US, GetEvTimeUs() },
               { SC_COL_RCV_TIME, GetRcvTime() },
               { SC_COL_RCV_TIME_US, GetRcvTimeUs() },
               { SC_COL_INS_TIME, GetInsertTime() },
               { SC_COL_INS_TIME_US, GetInsertTimeUs() },
               { SC_COL_QUALITY, GetQuality() },
               { SC_COL_ERROR_DESC, GetErrorDesc() },
               { "DataObject", "AttributeEventData" } };

    if (value_r.size() > 0) // RO or RW
        j += { SC_COL_VALUE_R, value_r };

    if (value_w.size() > 0) // RW or WO
        j += { SC_COL_VALUE_W, value_w };

    if (ttl != 0)
        j += { "_ttl", { "enabled", true, "default", ttl + "ms" } };

    return j.dump();
}

string HDBPP::AttributeEventData::ToElkScript4Update()
{
    throw string("The data object 'AttributeEventData' should not be updated in the DB");
}

string HDBPP::AttributeEventData::GetJsonQuery()
{
    throw string("The data object 'AttributeEventData' should not be searched in the DB");
}

void HDBPP::Document::SetParameterFromJson(json& p_json)
{
    throw string("The data object 'AttributeEventData' should not be read from the DB");
}

string HDBPP::Document::ToJson()
{
    json j = { { "datetime", datetime },
               { "domain", domain },
               { "family", family },
               { "member", member },
               { "name", name },
               { "host", host },
               { SC_COL_QUALITY, quality },
               { SC_COL_ERROR_DESC, errorDesc },
               { "DataObject", "Document" } };

    ostringstream valuekeyname;
    valuekeyname << name << ".";
    if (value_r.size() > 0) // RO or RW
    {
        valuekeyname << SC_COL_VALUE_R;
        j += { valuekeyname.str(), value_r };
    }
    if (value_w.size() > 0) // RW or WO
    {
        valuekeyname << SC_COL_VALUE_W;
        j += { valuekeyname.str(), value_w };
    }

    if (ttl != 0)
        j += { "_ttl", { "enabled", true, "default", ttl + "ms" } };

    return j.dump();
}

string HDBPP::Document::ToElkScript4Update()
{
    throw string("The data object 'AttributeEventData' should not be read from the DB");
}

string HDBPP::Document::GetJsonQuery()
{
    throw string("The data object 'AttributeEventData' should not be read from the DB");
}

HDBPP::Document::~Document()
{
}

HDBPP::Document::Document(string phost,
                          string pdomain,
                          string pfamily,
                          string pmember,
                          string pname,
                          int pttl,
                          string pdatetime,
                          int pquality,
                          string perrorDesc,
                          json pvalue_r,
                          json pvalue_w)
{
    host = phost;
    domain = pdomain;
    family = pfamily;
    member = pmember;
    name = pname;
    ttl = pttl;
    datetime = pdatetime;
    quality = pquality;
    errorDesc = perrorDesc;
    value_r = pvalue_r;
    value_w = pvalue_w;
}

HDBPP::Document::Document(AttributeConfiguration attr_conf, AttributeEventData eventData, string pdatetime)
{
    host = attr_conf.GetFacility();
    domain = attr_conf.GetDomain();
    family = attr_conf.GetFamily();
    member = attr_conf.GetMember();
    name = attr_conf.GetName();
    ttl = attr_conf.GetTtl();
    datetime = pdatetime;
    quality = eventData.GetQuality();
    errorDesc = eventData.GetErrorDesc();
    value_r = eventData.GetValueR();
    value_w = eventData.GetValueW();
}