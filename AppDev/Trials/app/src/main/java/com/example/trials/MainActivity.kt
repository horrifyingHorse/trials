package com.example.trials

import android.content.Intent
import android.graphics.Color
import android.os.Bundle
import android.text.Editable
import android.text.TextWatcher
import android.util.Log
import android.widget.Button
import android.widget.EditText
import android.widget.Toast
import androidx.activity.enableEdgeToEdge
import androidx.appcompat.app.AppCompatActivity
import androidx.core.view.ViewCompat
import androidx.core.view.WindowInsetsCompat
import com.example.trials.SignUpActivity
import okhttp3.Call
import okhttp3.Callback
import okhttp3.MediaType.Companion.toMediaType
import okhttp3.OkHttpClient
import okhttp3.Request
import okhttp3.RequestBody.Companion.toRequestBody
import okhttp3.Response
import okio.IOException
import org.json.JSONObject

class MainActivity : AppCompatActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        enableEdgeToEdge()
        setContentView(R.layout.activity_main)
        ViewCompat.setOnApplyWindowInsetsListener(findViewById(R.id.main)) { v, insets ->
            val systemBars = insets.getInsets(WindowInsetsCompat.Type.systemBars())
            v.setPadding(systemBars.left, systemBars.top, systemBars.right, systemBars.bottom)
            insets
        }

        val prefs = getSharedPreferences("MyAppPrefs", MODE_PRIVATE)
        if (prefs.getBoolean("isLoggedIn", false)) {
            val intent = Intent(this@MainActivity, HomeActivity::class.java)
            startActivity(intent)
        }

        val etUsername = findViewById<EditText>(R.id.etUserName);
        val etPassword = findViewById<EditText>(R.id.etPassword);
        val btnSubmitLogin = findViewById<Button>(R.id.btSubmitLogIn);
        val btnSignUp = findViewById<Button>(R.id.btSignUp);

        val url = "http://10.0.2.2:3000"
        val client = OkHttpClient()

        addTextListener(etUsername)
        addTextListener(etPassword)

        btnSubmitLogin.setOnClickListener {
            if (!validateField(etUsername, "Username") || !validateField(etPassword, "Password")) {
                return@setOnClickListener
            }

            val jsonObj = JSONObject().apply {
                put("username", etUsername.text.toString())
                put("password", etPassword.text.toString())
            }
            val body = jsonObj.toString().toRequestBody("application/json".toMediaType())
            val req = Request.Builder()
                .url("$url/signin")
                .post(body)
                .build()

            Log.d("App", "Making request")
            client.newCall(req).enqueue(object: Callback {
                override fun onFailure(call: Call, e: IOException) {
                    Log.e("Network", "Failed request {${e.message}")
                }
                override fun onResponse(call: Call, response: Response) {
                    val responseData = response.body?.string()
                    val json = JSONObject(responseData)
                    Log.d("Network", "response received: ${json.getString("success")}")

                    runOnUiThread{
                        val errorCode = json.getInt("errorcode")
                        if (errorCode == 1) {
                            etUsername.setBackgroundColor(0xFFF6D6D6.toInt())
                        } else if (errorCode == 2) {
                            etPassword.setBackgroundColor(0xFFF6D6D6.toInt())
                        }
                        Toast.makeText(this@MainActivity, "${json.getString("msg")}", Toast.LENGTH_SHORT).show();
                    }
                    if (json.getBoolean("success")) {
                        prefs.edit().putBoolean("isLoggedIn", true).apply()
                        prefs.edit().putString("username", etUsername.text.toString()).apply()
                        val intent = Intent(this@MainActivity, HomeActivity::class.java)
                        startActivity(intent)
                    }
                }
            })
        }

        btnSignUp.setOnClickListener {
            Toast.makeText(this, "Noob didn't even Register yet", Toast.LENGTH_LONG).show();
            val intent = Intent(this, SignUpActivity::class.java)
            startActivity(intent)
        }
    }


    fun validateField(field: EditText, text: String): Boolean {
        val str = field.text.toString()
        if (str.isEmpty()) {
            runOnUiThread {
                field.setBackgroundColor(0xFFF6D6D6.toInt())
                Toast.makeText(this@MainActivity, "$text with $str field cannot be empty!", Toast.LENGTH_SHORT).show();
            }
            return false
        }
        if (str.indexOf(" ") != -1) {
            runOnUiThread {
                field.setBackgroundColor(0xFFF6D6D6.toInt())
                Toast.makeText(this@MainActivity, "$text with $str cannot contain whitespaces", Toast.LENGTH_SHORT).show();
            }
            return false
        }
        return true
    }

    fun addTextListener(field: EditText) {
        field.addTextChangedListener(object: TextWatcher {
            override fun afterTextChanged(s: Editable) { }
            override fun beforeTextChanged(s: CharSequence?, start: Int, count: Int, after: Int) { }
            override fun onTextChanged(s: CharSequence?, start: Int, before: Int, count: Int) {
                field.setBackgroundColor(Color.TRANSPARENT)
            }
        })
    }
}