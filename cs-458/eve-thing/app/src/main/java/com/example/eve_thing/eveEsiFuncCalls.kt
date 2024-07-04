package com.example.eve_thing

import android.util.Log
import com.android.volley.Request
import com.android.volley.toolbox.JsonObjectRequest
import org.json.JSONObject

fun lookupID(name: String): Any? {
    val url = "https://esi.evetech.net/latest/universe/ids/?datasource=tranquility&language=en"
    val params = JSONObject()
    params.put("id", "The Forge")
    val request = JsonObjectRequest(
        Request.Method.POST,
        url,
        params,
        { response ->
            Log.d("reqponse", response.toString())
            //return@JsonObjectRequest response
        },
        { err ->
            Log.d("error", err.toString())
            //return@JsonObjectRequest err
        })

    return TODO("Provide the return value")
}

/*fun fetchDataOnItem(id: String?, orderType:String, regionID:Int, context: Context) {
    val url = "https://esi.evetech.net/latest/markets/"+
            regionID.toString()+
            "/orders/?datasource=tranquility&order_type="+
            orderType+
            "&page=1&type_id="+
            id.toString()
    Log.d("request url",url)

    val params = JSONObject()
    params.put("accept", "application/json")
    params.put("Cache-Control", "no-cache")

    val request = JsonArrayRequest(
        Request.Method.GET,
        url,
        null,
        { resopnse ->
            Log.d("json response fetch", resopnse.toString())
        },
        { err ->
            Log.d("json error fetch", err.toString())
        }
    )

    val que = Volley.newRequestQueue(context)
    que.add(request)
}*/

fun getValue(data: Map<String, String>, target: String): String? {

    var id: Int? = null

    val entries: List<Pair<String, String>> = data.toList()
    Log.d("list data", entries[0].first.toString())
    for (ent in entries) {
        if (target in ent.first) {
            if (id == null) {
                id = ent.second.toInt()
            } else if (ent.second.toInt() < id) {
                id = ent.second.toInt()
            }
        }
    }
    return id.toString()
}