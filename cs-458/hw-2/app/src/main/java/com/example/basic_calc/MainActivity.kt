package com.example.basic_calc

import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.view.View
import android.widget.Button
import android.widget.TextView

class MainActivity : AppCompatActivity() {
    var current_val = 0
    var num_one = 0
    var opperator = "none"

    fun update_dispaly() {
        val output = findViewById<TextView>(R.id.textbox)
        output.text = current_val.toString()
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        update_dispaly()
    }

    fun clear_pressed(view: View) {
        current_val = 0
        num_one = 0
        opperator = "none"
        update_dispaly()
    }

    fun num_press(view: View) {
        val button = view as Button
        val digit = button.text.toString().toInt()
        current_val *= 10
        current_val += digit
        update_dispaly()
    }

    fun opperator_pressed(view: View) {
        num_one = current_val
        val button = view as Button
        opperator = button.text.toString()
        current_val = 0
        update_dispaly()
    }

    fun enter_pressed(view: View) {
        if (opperator == "+") {
            current_val += num_one
        } else if (opperator == "-") {
            current_val -= num_one
        } else if (opperator == "/") {
            current_val /= num_one
        } else if (opperator == "*") {
            current_val *= num_one
        }

        update_dispaly()
        // remove to handle chains of opperations
        // ui is kind of wierd wihtout though as you can extend returned numbers
        current_val = 0
    }
}