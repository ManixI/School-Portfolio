package com.example.eve_thing

import android.annotation.SuppressLint
import android.content.Context
import android.os.Bundle
import android.util.Log
import android.view.View
import android.widget.Button
import android.widget.TextView
import androidx.appcompat.app.AppCompatActivity
import com.android.volley.Request
import com.android.volley.toolbox.JsonArrayRequest
import com.android.volley.toolbox.Volley
import org.json.JSONArray
import org.json.JSONObject

class MainActivity : AppCompatActivity() {
    /**
     * id: 10000002,
     * name: "The Forge"
     *
     * id: 30000142
     * system: Jita
      */

    var market_data: JSONArray? = null
    var status = "buy"
    var data: Map<String, String> = mutableMapOf<String, String>()
    var itemData: JSONArray? = null

    private fun swapStatus(): View.OnClickListener? {
        val buyButton = findViewById<Button>(R.id.buySellButton)

        if (status == "buy") {
            status = "sell"
            buyButton.text = "Sell"
        } else {
            status = "buy"
            buyButton.text = "Buy"
        }
        return null
    }

    fun setupBuyButton() {
        val buyButton = findViewById<Button>(R.id.buySellButton)
        buyButton.setOnClickListener{
            if (status == "buy") {
                status = "sell"
                buyButton.text = "Sell"
            } else {
                status = "buy"
                buyButton.text = "Buy"
            }
        }
    }

    @SuppressLint("SetTextI18n")
    fun testFetch() {

        val button = findViewById<Button>(R.id.testButton)

        button.setOnClickListener {
            Log.d("test button", "test button")
            val box = findViewById<TextView>(R.id.itemNameBox)
            val item = box.text
            Log.d("test button", item.toString())

            val display = findViewById<TextView>(R.id.showData)

            val id = getValue(data, item.toString())

            val url = "https://esi.evetech.net/latest/markets/"+
                    "10000002"+
                    "/orders/?datasource=tranquility&order_type="+
                    status+
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
                { response ->
                    Log.d("json response fetch", response.toString())
                    itemData = response
                    display.text = readResponse(response)
                },
                { err ->
                    Log.d("json error fetch", err.toString())
                    display.text = "Something went wrong: \n ${err.toString()}"
                }
            )

            val que = Volley.newRequestQueue(this)
            que.add(request)
        }
    }

    private fun readResponse(response: JSONArray?): String {
        if (response == null || response.length() == 0) {
            return "Error finding that item"
        }

        var text = ""
        var values = mutableListOf<List<Float>>()

        for (i in 0 until response.length()) {
            val obj = response.getJSONObject(i)
            val qty = obj.optString("volume_remain")
            val cost = obj.optString("price")

            values.add(listOf(cost.toFloat(), qty.toFloat()))

            //text += "Price: "+cost+" quantity: "+qty+"\n"
        }
        values.sortBy {it[0]}
        if (status == "buy") {
            values.reverse()
        }
        Log.d("sorted values", values.toString())

        for (ent in values) {
            text += "Price: "+ent[0].toString()+" quantity: "+ent[1].toString()+"\n"
        }

        return text
    }

    fun fetchDataOnItem(id: String?, orderType:String, regionID:Int, context: Context) {
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
                market_data = resopnse
            },
            { err ->
                Log.d("json error fetch", err.toString())
            }
        )

        val que = Volley.newRequestQueue(context)
        que.add(request)
    }




    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        var context = applicationContext
        Log.d("files list", context.assets.toString())
        context.assets.open("item-types.csv")


        //val data = readCSV(FileOutputStream("/assets/item-types.csv"))
        data = readCSV(context, "item-types.csv")
        /*for ((key, value) in data) {
            //Log.d("static test", "$key $value")
            if (key.toString() == "Condor Guristas SKIN") {
                Log.d("static data test4", "is in map")
            }
        }*/

        /*data.map { (K, V) ->
            Log.d("data values", "$K $V")
            Log.d("dsta values test", data[K]!!)
            }*/

        /*val entries: List<Pair<String, String>> = data.toList()
        Log.d("list data", entries[0].first.toString())
        for (ent in entries) {
            if ("Plagioclase" in ent.first) {
                Log.d("list data", ent.toString())
            }
        }*/

        //val id = getValue(data, "Plagioclase")
        //Log.d("return value", id.toString())

        /* none of these work, idk why
        Log.d("static data", data.toString())
        Log.d("static value test1", data.get("plagioclase").toString())
        Log.d("static value test2", data["Plagioclase"].toString())
        Log.d("static value test", getValue(data, "Plagioclase").toString())
        Log.d("static value test3", data.getValue(458.toString()))
        Log.d("static value test", data.getValue("Plagioclase")) */

        testFetch()
        //fetchDataOnItem(getValue(data, "Plagioclase"), status, 10000002, context)
        setupBuyButton()
    }
}