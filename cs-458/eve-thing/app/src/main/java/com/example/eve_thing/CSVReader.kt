package com.example.eve_thing

import android.content.Context
import android.util.Log
import java.io.File

public fun readCSV(context: Context, path: String): Map<String, String> {
    val inputStream = context.assets.open(path)
    //val inputStream = application.assets.open("myText.txt")
    val reader = inputStream.bufferedReader()
    // header of csv, can be ignored for now
    val header = reader.readLine()

    var data = mutableMapOf<String, String>()

    var line = reader.readLine()
    while(line != null) {
        val lineData = line.split(',')
        if (lineData[9] == "1") {
            data[lineData[2]] = lineData[0].toString()
            //Log.d("tmp data full", line)
            //Log.d("tmp data", "$lineData[1] $lineData[2]")
        }
        line = reader.readLine()
        /*reader.lineSequence()
            .map {
                val (unused, id, name, extra) = it.split(',', limit = 4)
                data[id] = name
                Log.d("tmp data", "$id $name")
            }*/
    }
    return data
}