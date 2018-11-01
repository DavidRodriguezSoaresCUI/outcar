package org.libsdl.app;

import android.content.Intent;
import android.content.res.Resources;
import android.os.Bundle;
import android.os.Environment;
import android.app.Activity;
import android.view.View;
import android.widget.*;

import java.io.*;

import com.outcar.game.OutCar;
import com.outcar.game.R;


public class MainActivity extends Activity {
    private Boolean saveInfo_gathered;
    private String pers_info_file, game_config_file;
    private Resources Res;
    private int pressBackToExit = 0, language;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        // Variable initialisation
        int scrWidth  = getWindowManager().getDefaultDisplay().getWidth(); // Get screen width
        saveInfo_gathered = false;
        language = -1;
        String ident = null;

        // Link to resources, used for convenience and because it's shorter
        Res = getResources();

        // Get Views from the text box and the three buttons
        TextView displaySave_Txt    = (TextView) findViewById( R.id.text_SavedInfo );
        Button   chooseLanguage_Btn = (Button)   findViewById( R.id.ChooseLanguageButton );
        Button   runGame_Btn        = (Button)   findViewById( R.id.RunGameButton );
        Button   delSave_Btn        = (Button)   findViewById( R.id.DelSaveButton );
        Button   runTutorial_Btn    = (Button)   findViewById( R.id.TutorialButton );

        // Creates strings to a folder rooted in the main storage, and to a file inside it
        // This file contains language and personal information about the player
        String pers_info_file_folder = Environment.getExternalStorageDirectory().toString()
                                       + File.separator + Res.getString( R.string.app_name );
        pers_info_file =  pers_info_file_folder + File.separator
                + Res.getString( R.string.pers_info_file );
        game_config_file =  pers_info_file_folder + File.separator
                + Res.getString( R.string.game_config_file );

        // Sets width of onscreen layout elements
        displaySave_Txt.setWidth(    (int) Math.floor( scrWidth * 0.50 ) );
        chooseLanguage_Btn.setWidth( (int) Math.floor( scrWidth * 0.41 ) );
        runGame_Btn.setWidth(        (int) Math.floor( scrWidth * 0.41 ) );
        delSave_Btn.setWidth(        (int) Math.floor( scrWidth * 0.41 ) );
        runTutorial_Btn.setWidth(    (int) Math.floor( scrWidth * 0.41 ) );

        // Reads the file containing personal information
        File savedInfo_file = new File( pers_info_file );
        if ( savedInfo_file.exists() ) {
            // The file exists, we try to read expected information
            try {
                BufferedReader reader = new BufferedReader( new FileReader(savedInfo_file) );
                try {
                    String line;
                    if ( (line = reader.readLine()) != null ) {
                        // Success ! 'data' holds the CSV data from the first line
                        String[] data = line.split("," );
                        // We retrieve relevant information and save that fact in 'saveInfo_gathered'
                        language = Integer.valueOf( data[0] ); // 'String -> int' conversion
                        ident    = data[1];
                        saveInfo_gathered = true;
                    }
                    reader.close(); // The reader is closed
                } catch ( IOException e2 ) {
                    e2.printStackTrace(); // Dealing with exceptions ..
                }
            } catch ( FileNotFoundException e ) {
                e.printStackTrace(); // Dealing with exceptions ..
            }

            // A message is displayed  in 'displaySave_Txt' TextView, to inform the user whether
            // their personal information was retrieved successfully
            String displayedInfo;
            if ( 0 <= language && language <= 2 && ident != null )
                displayedInfo = Res.getStringArray( R.array.infoFound )[language] + ident;
            else
                displayedInfo = Res.getString( R.string.saved_pers_error );

            displaySave_Txt.setText( displayedInfo );
        }

        // If the language information was retrieved, the buttons are translated
        if ( saveInfo_gathered ) {
            chooseLanguage_Btn.setText( Res.getStringArray( R.array.button_editInfo ) [language] );
            runGame_Btn.setText(        Res.getStringArray( R.array.button_startGame )[language] );
            runTutorial_Btn.setText( Res.getStringArray( R.array.button_startTutorial)[language] );
            delSave_Btn.setText(        Res.getStringArray( R.array.button_delSave )  [language] );
        } else {
            // If not, the 'personal information gathering' process begins
            startChooseLanguageActivity();
        }
    }

    public void onChooseLanguageButtonClick(View v) {
        // The user wants to edit its info
        startChooseLanguageActivity();
    }

    public void onRunGameButtonClick(View v) {
        // The user wants to start the game
        // We make sure that the configuration file exists
        File game_config = new File( game_config_file );
        if ( !game_config.exists() )
        {
            try{
                game_config.createNewFile();
                BufferedWriter writer = new BufferedWriter( new FileWriter( game_config ));
                try {
                    writer.append( Res.getString( R.string.game_config_file_1stline ) );
                    writer.append( '\n' );
                    writer.append( Res.getString( R.string.game_config_file_default_values ) );
                    writer.close();
                } catch ( Exception e ) {
                    e.printStackTrace(); // Dealing with exceptions ..
                }
            } catch ( IOException e ) {
                e.printStackTrace(); // Dealing with exceptions ..
            }
        }

        Intent runGame = new Intent( MainActivity.this, OutCar.class );
        runGame.addFlags( Intent.FLAG_ACTIVITY_NO_ANIMATION ); // To avoid lag from animations
        startActivity( runGame );
    }

    public void onDelSaveButtonClick(View v) {
        // The user wants to erase their personal information
        File savedInfo_file = new File( pers_info_file );
        if ( savedInfo_file.exists() ) {
            try {
                if ( savedInfo_file.delete() ) {
                    // Success ! Restart the Activity
                    MainActivity.this.recreate();
                } else {
                    // This shouldn't happen (not being able to delete the file), throws an error
                    // Note: Toast.LENGTH_SHORT ~2.5s and Toast.LENGTH_LONG ~4.0s
                    Toast.makeText( MainActivity.this,
                            Res.getString( R.string.std_error_msg ), Toast.LENGTH_LONG ).show();
                }
            } catch ( SecurityException e ) {
                e.printStackTrace(); // Dealing with exceptions ..
            }
        }
    }

    public void onTutorialButtonClick(View v) {
        Intent runTutorial = new Intent( MainActivity.this, Tutorial.class );
        runTutorial.addFlags( Intent.FLAG_ACTIVITY_NO_ANIMATION ); // To avoid lag from animations
        startActivity( runTutorial.setType( "text/plain" ).putExtra( "language", language )
                .putExtra( "skippable", 1 ) );
        finish();
    }

    @Override
    public void onBackPressed() {
        // If the 'back' button is pressed once, a message tells the user to press again to go to
        // your home screen, which happens if the user the button is pressed again.
        if ( ++pressBackToExit < 2 ) {
            Toast.makeText( MainActivity.this ,
                    Res.getString( R.string.press2exit ),
                    Toast.LENGTH_SHORT ).show();
        } else {
            // Puts the app to the background
            // TODO: determine whether to replace this 'soft exit' with closing the app
            MainActivity.this.moveTaskToBack(true);
        }
    }

    private void startChooseLanguageActivity() {
        // We quit 'MainActivity' and launch 'ChooseLanguage' Activity, passing whether it can come
        // back afterward (can't come back if personal information wasn't successfully
        // retrieved because there isn't a valid language selected.
        Intent runCL = new Intent( MainActivity.this, ChooseLanguage.class );
        runCL.setType( "text/plain" );
        runCL.putExtra( "saveInfo_gathered", saveInfo_gathered );
        runCL.putExtra( "language", language );
        startActivity( runCL );
        finish(); // Closes the current instance of 'MainActivity'
    }

}
