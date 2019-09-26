#include "LibHdb++ELK.h"

const string LOCAL_DC_DEFAULT = "DC1";

namespace HDBPP
{

//=============================================================================
//=============================================================================
void HdbPPELK::string_vector2map(vector<string> str, string separator, map<string, string>* results)
{
    for (vector<string>::iterator it = str.begin(); it != str.end(); it++) {
        string::size_type found_eq;
        found_eq = it->find_first_of(separator);

        if (found_eq != string::npos && found_eq > 0)
            results->insert(make_pair(it->substr(0, found_eq), it->substr(found_eq + 1)));
    }
}

HdbPPELK::HdbPPELK(vector<string> configuration)
{
    map<string, string> libhdb_conf;
    string_vector2map(configuration, "=", &libhdb_conf);
    // string elk_http_repo; string m_keyspace_name;
    string contact_points; //, user, password;
    string local_dc = LOCAL_DC_DEFAULT;

    // ---- logging_enabled optional config parameter ----
    try
    {
        string logging_enabled_state = libhdb_conf.at("logging_enabled");
        logging_enabled_state == "true" ? Log::LogLevel() = Debug : Log::LogLevel() = Error;
    }
    catch (const std::out_of_range& e)
    {
        // setting logging to Error by default
        Log::LogLevel() = Error;
    }

    // ---- Mandatory configuration parameters ----
    try
    {
        contact_points = libhdb_conf.at("contact_points");
        LOG(Debug) << "Configuration \"contact_points\" set to: " << contact_points << endl;
    }
    catch (const std::out_of_range& e)
    {
        stringstream error_desc;

        error_desc << "Configuration parsing error: \"contact_points\" mandatory configuration parameter not found"
                   << ends;

        LOG(Error) << error_desc.str() << endl;
        Tango::Except::throw_exception(EXCEPTION_TYPE_CONFIG, error_desc.str(), __func__);
    }

    try
    {
        m_keyspace_name = libhdb_conf.at("keyspace");
        LOG(Debug) << "contact_points=\"" << contact_points << "\"" << endl;
        LOG(Debug) << "keyspace=\"" << m_keyspace_name << "\"" << endl;
    }
    catch (const std::out_of_range& e)
    {
        stringstream error_desc;

        error_desc << "Configuration parsing error: \"keyspace\" mandatory configuration parameter not found" << ends;

        LOG(Error) << error_desc.str() << endl;
        Tango::Except::throw_exception(EXCEPTION_TYPE_CONFIG, error_desc.str(), __func__);
    }

    try
    {
        elk_http_repo = libhdb_conf.at("elk_http_repo");
        _DAL = new DAL(elk_http_repo);
    }
    catch (const std::out_of_range& e)
    {
        stringstream error_desc;

        error_desc << "Http url for Elastic repository must be present" << ends;

        LOG(Error) << error_desc.str() << endl;
        Tango::Except::throw_exception(EXCEPTION_TYPE_CONFIG, error_desc.str(), __func__);
    }
}

HdbPPELK::~HdbPPELK()
{
    TRACE_ENTER;

    delete (_DAL);

    TRACE_EXIT;
}

void HdbPPELK::insert_param_Attr(Tango::AttrConfEventData* data, HdbEventDataType ev_data_type)
{
    TRACE_ENTER;
    LOG(Warning) << "insert_param_Attr";

    if (data == NULL) {
        stringstream error_desc;
        error_desc << "Unexpected null Tango::AttrConfEventData" << ends;
        LOG(Error) << error_desc.str() << endl;
        Tango::Except::throw_exception(EXCEPTION_TYPE_NULL_POINTER, error_desc.str(), __func__);
    }

    if (data->attr_conf == NULL) {
        stringstream error_desc;
        error_desc << "Unexpected null in Tango::AttrConfEventData field attr_conf" << ends;
        LOG(Error) << error_desc.str() << endl;
        Tango::Except::throw_exception(EXCEPTION_TYPE_NULL_POINTER, error_desc.str(), __func__);
    }

    AttributeName attr_name(data->attr_name);
    AttributeConfiguration attr_conf(attr_name);

    int64_t ev_time = ((int64_t)data->get_date().tv_sec) * 1000;
    int ev_time_us = data->get_date().tv_usec;

    if (!_DAL->GetAttributeConfiguration(attr_conf)) {
        LOG(Error) << "Could not find ID for attribute " << attr_name << endl;
        stringstream error_desc;
        error_desc << "ERROR Could not find ID for attribute  \"" << attr_name << "\": " << ends;
        Tango::Except::throw_exception(EXCEPTION_TYPE_MISSING_ATTR, error_desc.str().c_str(), __func__);
    }

    LOG(Debug) << " label: \"" << data->attr_conf->label.c_str() << "\"" << endl;
    LOG(Debug) << " unit: \"" << data->attr_conf->unit.c_str() << "\"" << endl;
    LOG(Debug) << " standard unit: \"" << data->attr_conf->standard_unit.c_str() << "\"" << endl;
    LOG(Debug) << " display unit: \"" << data->attr_conf->display_unit.c_str() << "\"" << endl;
    LOG(Debug) << " format: \"" << data->attr_conf->format.c_str() << "\"" << endl;
    LOG(Debug) << " archive rel change: \"" << data->attr_conf->events.arch_event.archive_rel_change.c_str() << "\""
               << endl;
    LOG(Debug) << " archive abs change: \"" << data->attr_conf->events.arch_event.archive_abs_change.c_str() << "\""
               << endl;
    LOG(Debug) << " archive period: \"" << data->attr_conf->events.arch_event.archive_period.c_str() << "\"" << endl;
    LOG(Debug) << " description: \"" << data->attr_conf->description.c_str() << "\"" << endl;
    LOG(Debug) << " after binding description" << endl;

    // Get the current time
    struct timespec ts;
    if (clock_gettime(CLOCK_REALTIME, &ts) != 0)
        perror("clock_gettime()");

    int64_t insert_time = ((int64_t)ts.tv_sec) * 1000;
    int insert_time_us = ts.tv_nsec / 1000;

    AttributeParameter attr_param(attr_conf.GetID(),
                                  ev_time,
                                  ev_time_us,
                                  insert_time,
                                  insert_time_us,
                                  data->attr_conf->label.c_str(),
                                  data->attr_conf->unit.c_str(),
                                  data->attr_conf->standard_unit.c_str(),
                                  data->attr_conf->display_unit.c_str(),
                                  data->attr_conf->format.c_str(),
                                  data->attr_conf->events.arch_event.archive_rel_change.c_str(),
                                  data->attr_conf->events.arch_event.archive_abs_change.c_str(),
                                  data->attr_conf->events.arch_event.archive_period.c_str(),
                                  data->attr_conf->description.c_str());

    _DAL->SaveAttributeParameter(attr_param);

    TRACE_EXIT;
}

void HdbPPELK::insert_Attr(Tango::EventData* data, HdbEventDataType ev_data_type)
{
    TRACE_ENTER;

    if (data == NULL) {
        stringstream error_desc;
        error_desc << "Unexpected null Tango::EventData" << ends;
        LOG(Error) << error_desc.str() << endl;
        Tango::Except::throw_exception(EXCEPTION_TYPE_NULL_POINTER, error_desc.str(), __func__);
    }

    AttributeName attr_name(data->attr_name);
    AttributeConfiguration attr_conf(attr_name);

    int64_t ev_time;
    int ev_time_us;
    int64_t rcv_time = ((int64_t)data->get_date().tv_sec) * 1000;
    int rcv_time_us = data->get_date().tv_usec;
    int quality = (int)data->attr_value->get_quality();
    string error_desc("");

    Tango::AttributeDimension attr_w_dim;
    Tango::AttributeDimension attr_r_dim;
    int data_type = ev_data_type.data_type;
    Tango::AttrDataFormat data_format = ev_data_type.data_format;
    int write_type = ev_data_type.write_type;

    bool is_null = false;
    if (data->err) {
        LOG(Debug) << "Attribute in error:" << error_desc << endl;

        is_null = true;
        // Store the error description
        error_desc = data->errors[0].desc;
    }

    data->attr_value->reset_exceptions(Tango::DeviceAttribute::isempty_flag); // disable is_empty exception

    if (data->attr_value->is_empty()) {
        LOG(Debug) << "no value will be archived... (Attr Value is empty)" << endl;
        is_null = true;
    }

    if (quality == Tango::ATTR_INVALID) {
        LOG(Debug) << "no value will be archived... (Invalid Attribute)" << endl;
        is_null = true;
    }

    LOG(Debug) << "data_type=" << data_type << " data_format=" << data_format << " write_type=" << write_type << endl;

    if (!is_null) {
        attr_w_dim = data->attr_value->get_w_dimension();
        attr_r_dim = data->attr_value->get_r_dimension();
        ev_time = ((int64_t)data->attr_value->get_date().tv_sec) * 1000;
        ev_time_us = data->attr_value->get_date().tv_usec;
    } else {
        attr_r_dim.dim_x = 0; // max_dim_x;//TODO: OK?
        attr_w_dim.dim_x = 0; // max_dim_x;//TODO: OK?
        attr_r_dim.dim_y = 0; // max_dim_y;//TODO: OK?
        attr_w_dim.dim_y = 0; // max_dim_y;//TODO: OK?
        ev_time = rcv_time;
        ev_time_us = rcv_time_us;
    }

    if (!_DAL->GetAttributeConfiguration(attr_conf)) {
        LOG(Error) << "Could not find ID for attribute " << attr_name << endl;
        stringstream error_desc;
        error_desc << "ERROR Could not find ID for attribute  \"" << attr_name << "\": " << ends;
        Tango::Except::throw_exception(EXCEPTION_TYPE_MISSING_ATTR, error_desc.str().c_str(), __func__);
    }

    // Compute the period based on the month of the event time
    struct tm* tms;
    time_t ev_time_s = ev_time / 1000;

    if ((tms = localtime(&ev_time_s)) == NULL)
        perror("localtime");

    char period[11];
    snprintf(period, 11, "%04d-%02d-%02d", tms->tm_year + 1900, tms->tm_mon + 1, tms->tm_mday);

    // Get the current time
    struct timespec ts;
    if (clock_gettime(CLOCK_REALTIME, &ts) != 0)
        perror("clock_gettime()");

    int64_t insert_time = ((int64_t)ts.tv_sec) * 1000;
    int insert_time_us = ts.tv_nsec / 1000;

    json nulljson = {};
    AttributeEventData attr_event_data(attr_conf.GetID(),
                                       period,
                                       ev_time,
                                       ev_time_us,
                                       rcv_time,
                                       rcv_time_us,
                                       insert_time,
                                       insert_time_us,
                                       quality,
                                       error_desc.c_str(),
                                       nulljson,
                                       nulljson,
                                       attr_conf.GetTtl());

    if (write_type != Tango::WRITE) // RO or RW
        attr_event_data.SetValueR(extract_read(data, ev_data_type));

    if (write_type != Tango::READ) // RW or WO
        attr_event_data.SetValueW(extract_set(data, ev_data_type));

    if (!_DAL->SaveAttributeEventData(attr_event_data)) {
        stringstream error_desc;
        LOG(Error) << error_desc << "->SaveAttributeEventData ERROR! Json:\n" << _DAL->GetErrors() << endl;
        Tango::Except::throw_exception(EXCEPTION_TYPE_SAVEEVENTDATA, error_desc.str().c_str(), __func__);
    }

    Document doc(attr_conf, attr_event_data, asctime(tms));
    if (!_DAL->SaveDocument(doc)) {
        stringstream error_desc;
        LOG(Error) << error_desc << "->SaveDocument ERROR! Json:\n" << _DAL->GetErrors() << endl;
        Tango::Except::throw_exception(EXCEPTION_TYPE_SAVEEVENTDATA, error_desc.str().c_str(), __func__);
    }

    TRACE_EXIT;
}

void HdbPPELK::configure_Attr(string name, int type, int format, int write_type, unsigned int ttl)
{
    TRACE_ENTER;

    AttributeName attr_name(name);
    LOG(Debug) << "name=" << name << " -> tango_host=" << attr_name.tango_host_with_domain()
               << " attr_name=" << attr_name << endl;

    AttributeConfiguration attr_conf(attr_name);

    if (attr_name.validate_domain_family_member_name() != AttributeName::NoError) {
        stringstream error_desc;

        error_desc << attr_name.tango_host_with_domain() << "/" << attr_name
                   << " already configured with different configuration."
                   << "Please contact your administrator for this special case" << ends;

        LOG(Error) << error_desc.str() << endl;
        Tango::Except::throw_exception(EXCEPTION_TYPE_NULL_POINTER, error_desc.str(), __func__);
    }

    string data_type = get_data_type(type, format, write_type);
    struct timespec ts;
    if (clock_gettime(CLOCK_REALTIME, &ts) != 0) {
        perror("clock_gettime()"); // TODO handle this error?
    }
    int64_t current_time = ((int64_t)ts.tv_sec) * 1000;
    int current_time_us = ts.tv_nsec / 1000;

    bool already_configured = _DAL->GetAttributeConfiguration(attr_conf);

    if (!already_configured) {
        attr_conf.SetDataType(data_type);
        attr_conf.SetTtl(ttl);
        LOG(Error) << "Save Attribute Configuration " << attr_conf.ToJson() << endl;
        _DAL->SaveAttributeConfiguration(attr_conf);
        AttributeConfigurationHistory attr_conf_history(attr_conf, EVENT_ADD, current_time, current_time_us);
        LOG(Error) << "Save Attribute Configuration History" << attr_conf_history.ToJson() << endl;
        _DAL->SaveAttributeConfigurationHistory(attr_conf_history);

        TRACE_EXIT;
        return;
    }

    // if (already_configured) {
    if (attr_conf.GetDataType().compare(data_type) != 0) {
        stringstream error_desc;

        error_desc << attr_name.tango_host_with_domain() << "/" << attr_name
                   << " already configured with different configuration."
                   << "Please contact your administrator for this special case" << ends;

        LOG(Error) << error_desc.str() << endl;
        Tango::Except::throw_exception(EXCEPTION_TYPE_NULL_POINTER, error_desc.str(), __func__);
    }

    LOG(Debug) << "ALREADY CONFIGURED with same configuration: " << attr_name.tango_host_with_domain() << "/"
               << attr_name << endl;

    if (attr_conf.GetTtl() != ttl) {
        LOG(Debug) << ".... BUT different ttl: updating " << attr_conf.GetTtl() << " to " << ttl << endl;
        attr_conf.SetTtl(ttl);
        LOG(Debug) << "Save Attribute Configuration " << attr_conf.ToJson() << endl;
        _DAL->SaveAttributeConfiguration(attr_conf);
    }

    AttributeConfigurationHistory attr_conf_history(attr_conf);
    if (_DAL->GetAttributeConfigurationHistory(attr_conf_history)) {
        if (attr_conf_history.GetEventType().compare(EVENT_REMOVE) == 0) {
            LOG(Debug) << "Save Attribute Configuration History" << attr_conf_history.ToJson() << endl;
            _DAL->SaveAttributeConfigurationHistory(attr_conf_history);
        }
    }

    TRACE_EXIT;
}

void HdbPPELK::updateTTL_Attr(string fqdn_attr_name, unsigned int ttl)
{
    TRACE_ENTER;

    AttributeName attr_name(fqdn_attr_name);
    AttributeConfiguration attr_conf(attr_name);
    bool already_configured = _DAL->GetAttributeConfiguration(attr_conf);

    if (!already_configured) {
        stringstream error_desc;
        error_desc << "ERROR Attribute " << attr_name << " NOT FOUND in HDB++ configuration table" << ends;
        LOG(Error) << error_desc.str() << endl;
        Tango::Except::throw_exception(EXCEPTION_TYPE_MISSING_ATTR, error_desc.str(), __func__);
    }
    attr_conf.SetTtl(ttl);
    _DAL->SaveAttributeConfiguration(attr_conf);

    TRACE_EXIT;
}

void HdbPPELK::event_Attr(string fqdn_attr_name, unsigned char event)
{
    TRACE_ENTER;

    struct timespec ts;
    if (clock_gettime(CLOCK_REALTIME, &ts) != 0)
        perror("clock_gettime()");

    int64_t current_time = ((int64_t)ts.tv_sec) * 1000;
    int current_time_us = ts.tv_nsec / 1000;

    AttributeName attr_name(fqdn_attr_name);
    AttributeConfiguration attr_conf(attr_name);
    bool already_configured = _DAL->GetAttributeConfiguration(attr_conf);

    if (!already_configured) {
        stringstream error_desc;
        error_desc << "ERROR Attribute " << attr_name << " NOT FOUND in HDB++ configuration table" << ends;
        LOG(Error) << error_desc.str() << endl;
        Tango::Except::throw_exception(EXCEPTION_TYPE_MISSING_ATTR, error_desc.str().c_str(), __func__);
    }

    string event_name = "";
    AttributeConfigurationHistory attr_conf_history(attr_conf, event_name, current_time, current_time_us);

    switch (event) {
    case DB_START: {
        bool ret = _DAL->GetAttributeConfigurationHistory(attr_conf_history);
        string last_event = attr_conf_history.GetEventType();
        if (ret && last_event == EVENT_START) {
            // It seems there was a crash
            AttributeConfigurationHistory attr_conf_history_crash(
                attr_conf, EVENT_CRASH, current_time, current_time_us);
            _DAL->SaveAttributeConfigurationHistory(attr_conf_history_crash);
        }
        event_name = EVENT_START;
        break;
    }
    case DB_STOP: {
        event_name = EVENT_STOP;
        break;
    }
    case DB_REMOVE: {
        event_name = EVENT_REMOVE;
        break;
    }
    case DB_PAUSE: {
        event_name = EVENT_PAUSE;
        break;
    }
    default: {
        stringstream error_desc;
        error_desc << "ERROR for " << attr_name << " event=" << (int)event << " NOT SUPPORTED" << ends;
        LOG(Error) << error_desc.str() << endl;
        Tango::Except::throw_exception(EXCEPTION_TYPE_MISSING_ATTR, error_desc.str(), __func__);
    }
    }

    attr_conf_history.SetEventType(event_name);
    _DAL->SaveAttributeConfigurationHistory(attr_conf_history);

    TRACE_EXIT;
}

//=============================================================================
//=============================================================================
AbstractDB* HdbPPELKFactory::create_db(vector<string> configuration)
{
    return new HdbPPELK(configuration);
}

//=============================================================================
//=============================================================================
string HdbPPELK::get_data_type(int type /*DEV_DOUBLE, DEV_STRING, ..*/,
                               int format /*SCALAR, SPECTRUM, ..*/,
                               int write_type /*READ, READ_WRITE, ..*/) const
{
    TRACE_ENTER;

    ostringstream data_type;

    if (format == Tango::SCALAR) {
        data_type << TYPE_SCALAR << "_";
    } else {
        data_type << TYPE_ARRAY << "_";
    }

    switch (type) {
    case Tango::DEV_BOOLEAN:
        data_type << TYPE_DEV_BOOLEAN << "_";
        break;
    case Tango::DEV_UCHAR:
        data_type << TYPE_DEV_UCHAR << "_";
        break;
    case Tango::DEV_SHORT:
        data_type << TYPE_DEV_SHORT << "_";
        break;
    case Tango::DEV_USHORT:
        data_type << TYPE_DEV_USHORT << "_";
        break;
    case Tango::DEV_LONG:
        data_type << TYPE_DEV_LONG << "_";
        break;
    case Tango::DEV_ULONG:
        data_type << TYPE_DEV_ULONG << "_";
        break;
    case Tango::DEV_LONG64:
        data_type << TYPE_DEV_LONG64 << "_";
        break;
    case Tango::DEV_ULONG64:
        data_type << TYPE_DEV_ULONG64 << "_";
        break;
    case Tango::DEV_FLOAT:
        data_type << TYPE_DEV_FLOAT << "_";
        break;
    case Tango::DEV_DOUBLE:
        data_type << TYPE_DEV_DOUBLE << "_";
        break;
    case Tango::DEV_STRING:
        data_type << TYPE_DEV_STRING << "_";
        break;
    case Tango::DEV_STATE:
        data_type << TYPE_DEV_STATE << "_";
        break;
    case Tango::DEV_ENCODED:
        data_type << TYPE_DEV_ENCODED << "_";
        break;
    default:
        stringstream error_desc;
        error_desc << "(" << type << ", ...): Type not supported (" << __FILE__ << ":" << __LINE__ << ")" << ends;
        LOG(Error) << error_desc.str() << endl;
        Tango::Except::throw_exception(EXCEPTION_TYPE_UNSUPPORTED_ATTR, error_desc.str(), __func__);
    }

    if (write_type == Tango::READ) {
        data_type << TYPE_RO;
    } else {
        data_type << TYPE_RW;
    }

    TRACE_EXIT;
    return data_type.str();
}
//=============================================================================
//=============================================================================
json HdbPPELK::extract_read(Tango::EventData* data, HdbEventDataType ev_data_type)
{
    TRACE_ENTER;
    json result;

    vector<bool> vbool;
    vector<unsigned char> vUnsignedChar;
    vector<short> vshort;
    vector<unsigned short> vUshort;
    vector<int> vlong;
    vector<unsigned int> vUlong;
    vector<int64_t> vInt64;
    vector<uint64_t> cUInt64;
    vector<Tango::DevState> vState;
    vector<float> vfloat;
    vector<double> vdouble;
    vector<string> vString;

    int data_type = ev_data_type.data_type;
    Tango::AttrDataFormat data_format = ev_data_type.data_format;
    bool extract_success = false;
    // There is a read value
    switch (data_type) {
    case Tango::DEV_BOOLEAN:
        if (data->attr_value->extract_read(vbool)) {
            result["vbool"] = vbool;
            extract_success = true;
        };
    case Tango::DEV_UCHAR:
        if (data->attr_value->extract_read(vUnsignedChar)) {
            result["vUnsignedChar"] = vUnsignedChar;
            extract_success = true;
        }
    case Tango::DEV_SHORT:
        if (data->attr_value->extract_read(vshort)) {
            result["vshort"] = vshort;
            extract_success = true;
        }
        break;
    case Tango::DEV_USHORT:
        if (data->attr_value->extract_read(vUshort)) {
            result["vUshort"] = vUshort;
            extract_success = true;
        }
        break;
    case Tango::DEV_LONG: // tango://localhost:10000/sys/tg_test/1/no_value problem!
        if (data->attr_value->extract_read(vlong)) {
            result["vlong"] = vlong;
            extract_success = true;
        }
        break;
    case Tango::DEV_ULONG:
        if (data->attr_value->extract_read(vUlong)) {
            result["vUlong"] = vUlong;
            extract_success = true;
        }
        break;
    case Tango::DEV_LONG64:
        if (data->attr_value->extract_read(vInt64)) {
            result["vInt64"] = vInt64;
            extract_success = true;
        }
        break;
    case Tango::DEV_FLOAT:
        if (data->attr_value->extract_read(vfloat)) {
            result["vfloat"] = vfloat;
            extract_success = true;
        }
        break;
    case Tango::DEV_DOUBLE:
        if (data->attr_value->extract_read(vdouble)) {
            result["vdouble"] = vdouble;
            extract_success = true;
        }
        break;
    case Tango::DEV_STRING:
        if (data->attr_value->extract_read(vString)) {
            // try to parse as json??
            if (json::accept(vString[0])) {
                json j3 = json::parse(vString[0]);
                result["vJsonString"] = j3;
            } else {
                result["vString"] = vString;
            }
            // result["vString"] = vString;
            extract_success = true;
        }
        break;
    case Tango::DEV_STATE: {
        if (data_format == Tango::SCALAR) {
            // We cannot use the extract_read() method for the "State" attribute
            Tango::DevState st;
            *data->attr_value >> st;
            result["vState"] = (int)st;
            extract_success = true;
            break;
        }
        if (data->attr_value->extract_read(vState)) {
            result["vState"] = vState;
            extract_success = true;
        }
        break;
    }
    case Tango::DEV_ULONG64: {
        if (data->attr_value->extract_read(cUInt64)) {
            result["cUInt64"] = cUInt64;
            extract_success = true;
        }
        break;
    }
    default: {
        stringstream error_desc;
        error_desc << "Attribute " << data->attr_name << " type (" << (int)(data_type) << ")) not supported" << ends;
        LOG(Error) << error_desc.str() << endl;
        Tango::Except::throw_exception(EXCEPTION_TYPE_UNSUPPORTED_ATTR, error_desc.str(), __func__);
    }
    } // switch(data_type)

    if (!extract_success) {
        stringstream error_desc;
        error_desc << "Failed to extract the attribute " << data->attr_name
                   << " from the Tango EventData. Possible type mismatch?" << ends;
        LOG(Error) << error_desc.str() << endl;
        Tango::Except::throw_exception(EXCEPTION_TYPE_EXTRACT, error_desc.str(), __func__);
    }

    return result;
}

//=============================================================================
//=============================================================================
json HdbPPELK::extract_set(Tango::EventData* data, HdbEventDataType ev_data_type)
{
    TRACE_ENTER;
    json result;
    vector<bool> vbool;
    vector<unsigned char> vUnsignedChar;
    vector<short> vshort;
    vector<unsigned short> vUshort;
    vector<int> vlong;
    vector<unsigned int> vUlong;
    vector<float> vfloat;
    vector<double> vdouble;
    vector<uint64_t> cUInt64;
    vector<string> vString;
    vector<int64_t> vInt64;
    vector<Tango::DevState> vState;

    int data_type = ev_data_type.data_type;

    bool extract_success = false;
    // There is a read value
    switch (data_type) {
    case Tango::DEV_BOOLEAN:
        if (data->attr_value->extract_set(vbool)) {
            result["vbool"] = vbool;
            extract_success = true;
        };
    case Tango::DEV_UCHAR:
        if (data->attr_value->extract_set(vUnsignedChar)) {
            result["vUnsignedChar"] = vUnsignedChar;
            extract_success = true;
        }
    case Tango::DEV_SHORT:
        if (data->attr_value->extract_set(vshort)) {
            result["vshort"] = vshort;
            extract_success = true;
        }
        break;
    case Tango::DEV_USHORT:
        if (data->attr_value->extract_set(vUshort)) {
            result["vUshort"] = vUshort;
            extract_success = true;
        }
        break;
    case Tango::DEV_LONG:
        if (data->attr_value->extract_set(vlong)) {
            result["vlong"] = vlong;
            extract_success = true;
        }
        break;
    case Tango::DEV_ULONG:
        if (data->attr_value->extract_set(vUlong)) {
            result["vUlong"] = vUlong;
            extract_success = true;
        }
        break;
    case Tango::DEV_LONG64:
        if (data->attr_value->extract_set(vInt64)) {
            result["vInt64"] = vInt64;
            extract_success = true;
        }
        break;
    case Tango::DEV_ULONG64: {
        if (data->attr_value->extract_read(cUInt64)) {
            result["cUInt64"] = cUInt64;
            extract_success = true;
        }
    }
    case Tango::DEV_FLOAT:
        if (data->attr_value->extract_set(vfloat)) {
            result["vfloat"] = vfloat;
            extract_success = true;
        }
        break;
    case Tango::DEV_DOUBLE:
        if (data->attr_value->extract_set(vdouble)) {
            result["vdouble"] = vdouble;
            extract_success = true;
        }
        break;
    case Tango::DEV_STRING:
        if (data->attr_value->extract_set(vString)) {
            // try to parse as json??
            if (json::accept(vString[0])) {
                json j3 = json::parse(vString[0]);
                result["vJsonString"] = j3;
            } else {
                result["vString"] = vString;
            }
            // result["vString"] = vString;
            extract_success = true;
        }
        break;
    case Tango::DEV_STATE:
        if (data->attr_value->extract_set(vState)) {
            result["vState"] = vState;
            extract_success = true;
        }
        break;
    default: {
        stringstream error_desc;
        error_desc << "Attribute " << data->attr_name << " type (" << (int)(data_type) << ")) not supported" << ends;
        LOG(Error) << error_desc.str() << endl;
        Tango::Except::throw_exception(EXCEPTION_TYPE_UNSUPPORTED_ATTR, error_desc.str(), __func__);
    }
    } // switch(data_type)

    if (!extract_success) {
        stringstream error_desc;
        error_desc << "Failed to extract the attribute " << data->attr_name
                   << " from the Tango EventData. Possible type mismatch?" << ends;
        LOG(Error) << error_desc.str() << endl;
        Tango::Except::throw_exception(EXCEPTION_TYPE_EXTRACT, error_desc.str(), __func__);
    }

    return result;
}
// end Helper
}; // namespace

//=============================================================================
//=============================================================================
DBFactory* getDBFactory()
{
    HDBPP::HdbPPELKFactory* db_ELK_factory = new HDBPP::HdbPPELKFactory();
    return static_cast<DBFactory*>(db_ELK_factory);
}