package org.libsdl.app;

import android.app.AlertDialog;
import android.content.Intent;
import android.content.res.Resources;
import android.graphics.Point;
import android.os.Bundle;
import android.os.Environment;
import android.app.Activity;
import android.view.View;
import android.widget.*;

import java.io.*;

import com.outcar.game.R;

public class GatherInfo extends Activity {
    private EditText ask4ident_data, ask4age_data;
    private Spinner ask4gender_spinner, ask4hand_spinner;
    private int language;
    private boolean firstTime;
    private String pers_info_file_folder;
    private String pers_info_file;
    private Resources Res;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_gather_info);
        Res = getResources();

        Point point = new Point();
        getWindowManager().getDefaultDisplay().getSize( point );
        int scrWidth = point.x;

        pers_info_file_folder = Environment.getExternalStorageDirectory().toString()
                + File.separator + getString( R.string.app_name );
        pers_info_file =  pers_info_file_folder
                + File.separator + getString(R.string.pers_info_file);

        Intent oriIntent = getIntent();
        language = oriIntent.getIntExtra( "language", 0 );
        firstTime = true;

        TextView title, askInfoText, ask4ident, ask4age, ask4gender, ask4hand;

        title       = (TextView) findViewById( R.id.text_title );
        askInfoText = (TextView) findViewById( R.id.ask_info );
        ask4ident   = (TextView) findViewById( R.id.ask4ident );
        ask4age     = (TextView) findViewById( R.id.ask4age );
        ask4gender  = (TextView) findViewById( R.id.ask4gender );
        ask4hand    = (TextView) findViewById( R.id.ask4hand );

        ask4ident_data     = (EditText) findViewById( R.id.ask4ident_data );
        ask4age_data       = (EditText) findViewById( R.id.ask4age_data );
        ask4gender_spinner = (Spinner)  findViewById( R.id.ask4gender_spinner );
        ask4hand_spinner   = (Spinner)  findViewById( R.id.ask4hand_spinner );

        title.setText( String.format("%s %s",
                Res.getStringArray( R.array.welcome2ask4info )[language],
                Res.getString( R.string.ori_app_name )) );
        askInfoText.setText( Res.getStringArray( R.array.ask4info )[language] );

        ask4ident.setWidth( (int) Math.floor( scrWidth * 0.45 ) );
        ask4ident.setText( Res.getStringArray( R.array.ask4ident )[language] );
        ask4ident_data.setWidth( (int) Math.floor( scrWidth * 0.45 ) );
        ask4ident_data.setHint( R.string.identHint );

        ask4age.setWidth( (int) Math.floor( scrWidth * 0.45 ) );
        ask4age.setText( Res.getStringArray( R.array.ask4age )[language] );
        ask4age_data.setWidth( (int) Math.floor( scrWidth * 0.45 ) );
        ask4age_data.setHint( R.string.ageHint );

        ask4gender.setWidth( (int) Math.floor( scrWidth * 0.45 ) );
        ask4gender.setText( Res.getStringArray( R.array.ask4gender )[language] );
        changeSpinnerData( ask4gender_spinner, "gender" );

        ask4hand.setWidth( (int) Math.floor( scrWidth * 0.45 ) );
        ask4hand.setText( Res.getStringArray( R.array.ask4hand )[language] );
        changeSpinnerData( ask4hand_spinner, "hand" );

        restore_gathered_info();
    }

    private void restore_gathered_info() {
        try {
            File file = new File(pers_info_file);

            // Reads personal info from file
            if ( file.exists() ) {
                firstTime = false;

                BufferedReader csvReader = new BufferedReader(new FileReader( file ));

                String line = csvReader.readLine();

                String[] data = line.split( "," );

                ask4ident_data.setText( data[1].replace( " ", "" ) );
                ask4age_data.setText( data[2].replace( " ", "" ) );

                // ask4gender_spinner
                int pos = 0;
                String[] genderSTD = Res.getStringArray( R.array.genderSTD );
                String gender_stored = data[3].replace( " ", "" )
                        .replace( "\"", "" );
                for( int i = 0; i < 3; i++ )
                {
                    if ( gender_stored.equals( genderSTD[i] ) )
                    {
                        pos = i;
                        break;
                    }
                }
                ask4gender_spinner.setSelection( pos );

                // ask4hand_spinner
                pos = 0;
                String[] handSTD = Res.getStringArray( R.array.handSTD );
                String hand_stored = data[4].replace( " ", "" )
                        .replace( "\"", "" );
                for( int i = 0; i < 4; i++ )
                {
                    if ( hand_stored.equals( handSTD[i] ) )
                    {
                        pos = i;
                        break;
                    }
                }
                ask4hand_spinner.setSelection( pos );

                csvReader.close();
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    private void changeSpinnerData( Spinner spinner, String basename ) {
        // Uses the 'basename' to get spinner data from resources (strings.xml string-array)
        // ex: basename='hand', language=1 -> we get the french string-array 'hand1'
        int testid = Res.getIdentifier( basename
                     + String.valueOf( language ), "array", GatherInfo.this.getPackageName() );
        ArrayAdapter<CharSequence> spinner_adapter;
        if (testid != 0) {
            spinner_adapter = ArrayAdapter.createFromResource( this,
                    testid, android.R.layout.simple_spinner_dropdown_item);
        } else {
            // Error ! Spinner data wasn't found
            spinner_adapter = ArrayAdapter.createFromResource( this,
                    R.array.error, android.R.layout.simple_spinner_dropdown_item);
        }
        spinner.setAdapter( spinner_adapter );
    }

    public Boolean argumentsAreValid( int ident, int age, int gender_index, int hand_index  ) {
        Boolean valid = true;

        if ( ident < 1000 || 99999 < ident )
            valid = false;
        if ( age < 5 || 100 < age )
            valid = false;
        if ( gender_index < 1 || hand_index < 1 )
            valid = false;
        return valid;
    }

    public void onConfirmButtonClicked( View view ) {
        int ident = -1, age = -1;

        try {
            ident         = Integer.parseInt(ask4ident_data.getText().toString());
        } catch ( NumberFormatException e ) { e.printStackTrace(); }

        int magic_number = Res.getInteger( R.integer.magic_number );

        if ( ident == magic_number ) {
            send_database();
            return;
        }

        try {
            age = Integer.parseInt(ask4age_data.getText().toString());
        } catch (NumberFormatException e) {
            e.printStackTrace();
        }

        String gender = ask4gender_spinner.getSelectedItem().toString();
        int gender_index = ask4gender_spinner.getSelectedItemPosition();
        String gender_STD = (Res.getStringArray(R.array.genderSTD))[gender_index];

        String hand = ask4hand_spinner.getSelectedItem().toString();
        int hand_index = ask4hand_spinner.getSelectedItemPosition();
        String hand_STD = (Res.getStringArray(R.array.handSTD))[hand_index];

        Boolean validInfo = argumentsAreValid(ident, age, gender_index, hand_index);

        if (validInfo) {

            PrintWriter csvWriter;
            try {
                File folder = new File(pers_info_file_folder);
                if (!folder.exists()) {
                    folder.mkdir();
                }
                if (folder.exists()) {
                    File file = new File(pers_info_file);
                    Boolean delFileError = false;

                    if (file.exists()) {
                        if (!file.delete()) {
                            displayMessage("Error", "Couldn't delete file !"
                                    + '\n' + pers_info_file);
                            delFileError = true;
                        }
                    }

                    // Creates a file if it doesn't exist, with exception control
                    try {
                        file.createNewFile();
                    } catch (IOException e) {
                        e.printStackTrace();
                    }

                    // Writes personal info to file
                    if (file.exists() && !delFileError) {
                        csvWriter = new PrintWriter(new FileWriter(file, true));

                        String line2write = String.valueOf(language) + ", " + String.valueOf(ident)
                                + ", " + String.valueOf(age) + ", \"" + gender_STD + "\", \""
                                + hand_STD + "\"";
                        csvWriter.print(line2write);
                        csvWriter.append('\n');

                        csvWriter.close();
                    } else {
                        displayMessage("Error", "Couldn't create file !" + '\n' + pers_info_file);
                    }
                }

            } catch (Exception e) {
                e.printStackTrace();
            }

            if (firstTime) {
                startActivity(new Intent(GatherInfo.this, Tutorial.class)
                        .setType("text/plain").putExtra("language", language)
                        .addFlags(Intent.FLAG_ACTIVITY_NO_ANIMATION));
            } else {
                startActivity(new Intent(GatherInfo.this, MainActivity.class)
                        .addFlags(Intent.FLAG_ACTIVITY_NO_ANIMATION));
            }

            finish();

        } else {
            displayMessage("Error", Res.getStringArray(R.array.text_invalidInfo)[language]);
        }
    }

    private void send_database() {
        startActivity(new Intent(GatherInfo.this, Send_database.class)
                .setType("text/plain").putExtra("language", language)
                .addFlags(Intent.FLAG_ACTIVITY_NO_ANIMATION));
        finish();
    }

    private void displayMessage( String title, String text ) {
        AlertDialog.Builder message_builder = new AlertDialog.Builder( GatherInfo.this );
        message_builder.setMessage( text ).setTitle( title );
        AlertDialog dialog = message_builder.create();
        dialog.show();
    }

}
