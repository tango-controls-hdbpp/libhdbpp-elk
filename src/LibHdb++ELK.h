#include "Log.h"
#include "AttributeName.h"
#include "LibHdb++Defines.h"

#include <libhdb++/LibHdb++.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <iostream>
#include <vector>
#include <map>
#include <queue>
#include <stdint.h>
#include <tango.h>

#include "json/json.hpp"
using json = nlohmann::json;
#include "restclient-cpp/connection.h"
#include "restclient-cpp/restclient.h"
#include "DataObjects.h"
#include "DAL.h"

namespace HDBPP
{

class HdbPPELK : public AbstractDB
{
private:
    // cache the attribute name to some of its often used data, i.e. ttl and id. This
    // saves it being looked up in the database everytime we request it
    map<string, AttributeParams> attribute_cache;

    // ELK web site (no end slash!) i.e. 'http://197.234.23.1:9200' or '197.234.23.1:9200'
    string elk_http_repo;
    DAL* _DAL;
    string m_keyspace_name;

    string get_data_type(int type /*DEV_DOUBLE, DEV_STRING, ..*/,
                         int format /*SCALAR, SPECTRUM, ..*/,
                         int write_type /*READ, READ_WRITE, ..*/) const;

    void string_vector2map(vector<string> str, string separator, map<string, string>* results);

    json extract_read(Tango::EventData* data, HdbEventDataType ev_data_type);

    json extract_set(Tango::EventData* data, HdbEventDataType ev_data_type);

public:
    ~HdbPPELK();
    HdbPPELK(vector<string> configuration);
    
    /**
     * @brief Insert an attribute archive event into the database
     *
     * Inserts an attribute archive event for the EventData into the database. If the attribute
     * does not exist in the database, then an exception will be raised. If the attr_value
     * field of the data parameter if empty, then the attribute is in an error state
     * and the error message will be archived.
     *
     * @param data Tango event data about the attribute.
     * @param ev_data_type HDB event data for the attribute.
     * @throw Tango::DevFailed
     */
    virtual void insert_Attr(Tango::EventData* data, HdbEventDataType ev_data_type);

    /**
     * @brief Inserts the attribute configuration data.
     *
     * Inserts the attribute configuration data (Tango Attribute Configuration event data)
     * into the database. The attribute must be configured to be stored in HDB++,
     * otherwise an exception will be thrown.
     *
     * @param data Tango event data about the attribute.
     * @param ev_data_type HDB event data for the attribute.
     * @throw Tango::DevFailed
     */
    virtual void insert_param_Attr(Tango::AttrConfEventData* data, HdbEventDataType ev_data_type);

    /**
     * @brief Add and configure an attribute in the database.
     *
     * Trying to reconfigure an existing attribute will result in an exception, and if an
     * attribute already exists with the same configuration then the ttl will be updated if
     * different.
     *
     * @param fqdn_attr_name Fully qualified attribute name
     * @param type The type of the attribute.
     * @param format The format of the attribute.
     * @param write_type The read/write access of the type.
     * @param  ttl The time to live in hour, 0 for infinity
     * @throw Tango::DevFailed
     */
    virtual void configure_Attr(string fqdn_attr_name,
                                int type /*DEV_DOUBLE, DEV_STRING, ..*/,
                                int format /*SCALAR, SPECTRUM, ..*/,
                                int write_type /*READ, READ_WRITE, ..*/,
                                unsigned int ttl);

    /**
     * @brief Update the ttl value for an attribute.
     *
     * The attribute must have been configured to be stored in HDB++, otherwise an exception
     * is raised
     *
     * @param fqdn_attr_name Fully qualified attribute nam
     * @param ttl The time to live in hour, 0 for infinity
     * @throw Tango::DevFailed
     */
    virtual void updateTTL_Attr(string fqdn_attr_name, unsigned int ttl);

    /**
    * @brief Record a start, Stop, Pause or Remove history event for an attribute.
    *
    * Inserts a history event for the attribute name passed to the function. The attribute
    * must have been configured to be stored in HDB++, otherwise an exception is raised.
    * This function will also insert an additional CRASH history event before the START
    * history event if the given event parameter is DB_START and if the last history event
    * stored was also a START event.
    *
    * @param fqdn_attr_name Fully qualified attribute name
    * @param event
    * @throw Tango::DevFailed
    */
    virtual void event_Attr(string fqdn_attr_name, unsigned char event);
};

class HdbPPELKFactory : public DBFactory
{
public:
    virtual AbstractDB* create_db(vector<string> configuration);
};

}; // namespace
