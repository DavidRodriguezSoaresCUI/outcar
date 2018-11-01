package org.libsdl.app;

import android.app.Activity;
import android.content.Intent;
import android.content.res.Resources;
import android.os.Bundle;
import android.os.Environment;
import android.view.View;
import android.widget.TextView;
import android.widget.Toast;

import java.io.*;
import java.util.*;

import com.outcar.game.R;

public class SplashScreen extends Activity {
    private Timer ssDelay;
    private boolean paused, exitAfterDelay;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_splash_screen);

        // Link to resources, used for convenience and because it's shorter
        Resources Res = getResources();

        // Creates strings to a folder rooted in the main storage
        String main_folder = Environment.getExternalStorageDirectory().toString()
                + File.separator + Res.getString( R.string.app_name );

        paused = false;
        exitAfterDelay = false;

        int delay_ms = Res.getInteger(R.integer.default_splashscreen_delay);

        // A timer is set, in order for the splashscreen to end after 'delay_ms' milliseconds
        ssDelay = new Timer();
        ssDelay.schedule(new TimerTask() {
            @Override
            public void run() {
                fromSplashScreentoMainMenu();
            }
        }, delay_ms);

        File folder = new File( main_folder );
        if ( !folder.exists() ) {
            if( !folder.mkdir() ) {
                // Error: couldn't create the main folder
                Toast.makeText(SplashScreen.this, R.string.create_file_error,
                        Toast.LENGTH_LONG ).show();
                exitAfterDelay = true;
            }
        }
    }

    public void onSplashScreenClick(View v) {
        // The user wants to skip the splashscreen
        cancelTimer(); // Cancel the timer because it's redundant now
        fromSplashScreentoMainMenu();
    }

    private void cancelTimer() {
        ssDelay.cancel(); // Cancel the timer
    }

    private void fromSplashScreentoMainMenu() {
        // We quit 'SplashScreen' Activity and launch 'MainActivity'
        if ( !exitAfterDelay ) {
            Intent runMain = new Intent(SplashScreen.this, MainActivity.class);
            startActivity(runMain);
        }
        finish(); // Closes the current instance of 'SplashScreen'
    }

    @Override
    protected void onStop() {
        super.onStop();
        cancelTimer(); // Cancel the timer because the app is hidden now
    }

    @Override
    protected void onPause() {
        super.onPause();
        paused = true;
        cancelTimer(); // Cancel the timer because the app is hidden now
    }

    @Override
    protected void onResume() {
        super.onResume();
        if ( paused ) {
            paused = false;
            fromSplashScreentoMainMenu();
        }
    }
}
