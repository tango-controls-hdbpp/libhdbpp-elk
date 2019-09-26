curl -XPOST 'localhost:9200/osboxes/_delete_by_query?pretty' -H 'Content-Type: application/json' -d'
{
  "query": { 
    "match": {
      "_type": "att_conf"
    }
  }
}
'

