package com.example.apoddemo

import android.annotation.SuppressLint
import android.app.DatePickerDialog
import android.graphics.Bitmap
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.text.method.ScrollingMovementMethod
import android.util.Log
import android.view.View
import android.widget.Button
import android.widget.ImageView
import android.widget.ImageView.ScaleType
import android.widget.ImageView.ScaleType.*
import android.widget.TextView
import com.android.volley.Request
import com.android.volley.Response
import com.android.volley.toolbox.ImageRequest
import com.android.volley.toolbox.JsonObjectRequest
import com.android.volley.toolbox.StringRequest
import com.android.volley.toolbox.Volley
import java.util.Calendar

class MainActivity : AppCompatActivity() {
    private var baseUrl = "https://api.nasa.gov/planetary/apod?api_key=DEMO_KEY"

    @SuppressLint("SetTextI18n")
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        val desc = findViewById<TextView>(R.id.imageDescription)

        desc.movementMethod = ScrollingMovementMethod()

        val button = findViewById<Button>(R.id.dateButton)

        // ref: https://devofandroid.blogspot.com/2018/03/date-picker-dialog-kotlin-android-studio.html
        val c = Calendar.getInstance()
        val year = c.get(Calendar.YEAR)
        val month = c.get(Calendar.MONTH)
        val day = c.get(Calendar.DAY_OF_MONTH)

        button.setText("${month+1}-$day, $year")

        button.setOnClickListener {
            val dpd = DatePickerDialog(this, DatePickerDialog.OnDateSetListener {
                view, year, monthOfYear, dayOfMonth ->
                button.setText("${monthOfYear+1}-$dayOfMonth, $year")
            }, year, month, day)
            dpd.show()
            this.baseUrl = "https://api.nasa.gov/planetary/apod?api_key=DEMO_KEY&date="+year.toString()+
                    "-"+month.toString()+"-"+day.toString()
            Log.d("json request url", this.baseUrl)
        }

    }

    @SuppressLint("SetTextI18n")
    fun fetchInfo(view: View) {
        /*
            NOTE: you need to set the INTERNET permission in the manifest, and
            add Volley to your module gradle file as a dependency
         */

        val title = findViewById<TextView>(R.id.imageTitle)
        val copyright = findViewById<TextView>(R.id.imageCopyright)
        val desc = findViewById<TextView>(R.id.imageDescription)

        title.text = "Fetching"
        copyright.text = ""
        desc.text = ""

        val queue = Volley.newRequestQueue(this)

        var imgReq: ImageRequest

        val jsonRequest = JsonObjectRequest(Request.Method.GET, // what sort of request
            this.baseUrl, // url as a string
            null, // json data to throw at the server
            { response ->
                // a closure of what to do if the request succeeds
                Log.d("json data", response.toString());
                title.text = response.getString("title")    // note we don't need thread shenanigans
                desc.text = response.getString("explanation")
                Log.d("json url", response.has("url").toString())
                if (response.has("copyright")) {
                    copyright.text = response.getString("copyright")
                } else {
                    copyright.text = ""
                }
                if (response.getString("media_type") == "video") {
                    copyright.text = "Error: can't display videos"
                } else if (response.has("url")) {
                    imgReq = fetchImage(view, response.getString("url"))
                    queue.add(imgReq)
                } else {
                    copyright.text = "No image for this day"
                }

            },
            { error ->
                // what to do if things fail
                desc.text = "That didn't work: ${error.localizedMessage}"
            })

        queue.add(jsonRequest)
    }

    @SuppressLint("SetTextI18n")
    fun fetchImage(view: View, url: String): ImageRequest {
        val img = findViewById<ImageView>(R.id.image)
        val copyright = findViewById<TextView>(R.id.imageCopyright)

        Log.d("json url", url)
        Log.d("json image info", img.layoutParams.width.toString())
        Log.d("json image info", img.layoutParams.height.toString())

        val imgReq = ImageRequest(
            url,
            { response ->
                img.setImageBitmap(response)
            },
            img.layoutParams.width,
            img.layoutParams.height,
            CENTER_CROP,
            Bitmap.Config.ARGB_8888,
            { error ->
                Log.d("json img", "could not get image: ${error.localizedMessage}")
                copyright.text = "Could not download image ${error.localizedMessage}"
        })

        return imgReq
    }

    fun setDate(view: View) {
        val button = findViewById<Button>(R.id.dateButton)

        val c = Calendar.getInstance()
        val year = c.get(Calendar.YEAR)
        val month = c.get(Calendar.MONTH)
        val day = c.get(Calendar.DAY_OF_MONTH)
    }
}