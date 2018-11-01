package org.libsdl.app;

import android.content.Intent;
import android.os.Bundle;
import android.app.Activity;
import android.os.Handler;
import android.view.View;
import android.widget.*;

import com.outcar.game.R;

public class ChooseLanguage extends Activity {
    private Button chooseEnglish, chooseFrench, chooseGerman;
    private Handler handler;
    private Boolean canExitToMenu;
    private int pressBackToExit = 0, language;
    private static final int pushButtonDelay = 150;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_choose_language);

        canExitToMenu = getIntent().getBooleanExtra( "saveInfo_gathered", false );
        language      = getIntent().getIntExtra( "language", 0 );

        handler = new Handler();

        Button exitMenu;

        chooseEnglish = (Button) findViewById( R.id.imageButton1 );
        chooseFrench  = (Button) findViewById( R.id.imageButton2 );
        chooseGerman  = (Button) findViewById( R.id.imageButton3 );
        exitMenu      = (Button) findViewById( R.id.exitCL );

        TextView title, textEnglish, textFrench, textGerman;

        title       = (TextView) findViewById( R.id.text_title );
        textEnglish = (TextView) findViewById( R.id.imageButton1_desc );
        textFrench  = (TextView) findViewById( R.id.imageButton2_desc );
        textGerman  = (TextView) findViewById( R.id.imageButton3_desc );

        chooseEnglish.setBackgroundResource( R.drawable.button_blank_trans );
        chooseFrench.setBackgroundResource( R.drawable.button_blank_trans );
        chooseGerman.setBackgroundResource( R.drawable.button_blank_trans );

        String[] language_array = getResources().getStringArray(R.array.language);

        title.setText( R.string.ori_app_name );
        textEnglish.setText( language_array[0] );
        textFrench.setText( language_array[1] );
        textGerman.setText( language_array[2] );

        if ( !canExitToMenu )
            exitMenu.setVisibility( View.GONE );
        else
            exitMenu.setText( getResources().getStringArray(R.array.exit_to_main_menu)[language] );

        // Animates the button the user touched - listeners are simpler here than public functions
        chooseEnglish.setOnClickListener( new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                chosenLanguage( chooseEnglish, 0 );
            }
        });

        chooseFrench.setOnClickListener( new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                chosenLanguage( chooseFrench, 1 );
            }
        });

        chooseGerman.setOnClickListener( new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                chosenLanguage( chooseGerman, 2 );
            }
        });

        exitMenu.setOnClickListener( new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                goBackToMenu();
            }
        });
    }

    private void chosenLanguage(final Button button, final int language ) {
        pressBackToExit = 0; // this counter is reset
        // Changes the buttons background, as if it was pressed
        button.setBackgroundResource( R.drawable.button_blank_pushed_trans );

        handler.postDelayed(new Runnable() {
            public void run() {
                // Goes to GatherInfo activity and restores the buttons normal background
                // after delay defined with pushButtonDelay
                Intent runGI = new Intent( ChooseLanguage.this, GatherInfo.class );
                runGI.setType( "text/plain" );
                runGI.putExtra( "language", language );
                startActivity( runGI );
                button.setBackgroundResource( R.drawable.button_blank_trans );
            }
        }, pushButtonDelay);
    }

    @Override
    public void onBackPressed() {
        if ( canExitToMenu )
            goBackToMenu();
        else {
            if ( ++pressBackToExit < 2 ) {
                Toast toast = Toast.makeText( ChooseLanguage.this ,
                        getResources().getString( R.string.press2exit ),
                        Toast.LENGTH_SHORT );
                toast.show();
            } else {
                ChooseLanguage.this.moveTaskToBack(true);
            }
        }
    }

    private void goBackToMenu() {
        Intent runMenu = new Intent( ChooseLanguage.this, MainActivity.class );
        startActivity( runMenu );
        finish();
    }

}
