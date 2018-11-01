package org.libsdl.app;

import android.app.Activity;
import android.content.Intent;
import android.net.Uri;
import android.os.Environment;
import android.provider.Settings;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;

import com.outcar.game.R;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;

public class Send_database extends Activity {
    private Intent emailIntent;
    private String user_info_path;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_send_database);

        Intent oriIntent = getIntent();
        int language = oriIntent.getIntExtra( "language", 0 );

        Button sendEmailButton = (Button) findViewById( R.id.button_sendEmail );

        String pers_info_file_folder = Environment.getExternalStorageDirectory().toString()
                + File.separator + getString( R.string.app_name );
        String database_path_unformatted =  pers_info_file_folder
                + File.separator + getString(R.string.stat_db_file);
        user_info_path =  pers_info_file_folder
                + File.separator + getString(R.string.pers_info_file);

        // We personalise the personal info file's name, adding a 64-bit unique ID to that device.
        // Note: that ID changes when the device is wiped.
        String android_id =
                Settings.Secure.getString( getContentResolver(), Settings.Secure.ANDROID_ID );
        String database_path = String.format(database_path_unformatted, android_id);

        File database_file = new File( database_path );
        String user_ID = getUserID();

        if ( !database_file.exists() || user_ID == null )
        {
            sendEmailButton.setText(
                    getResources().getStringArray( R.array.send_database_error )[language] );

        } else {
            emailIntent = new Intent(Intent.ACTION_SEND);
            Uri path = Uri.fromFile( database_file );
            // set the type to 'email'
            emailIntent.setType("vnd.android.cursor.dir/email");
            // set the destination email address
            String[] to = { getString(R.string.email_dest_database) };
            String subject = getString(R.string.send_email_message_subject);
            emailIntent.putExtra(Intent.EXTRA_EMAIL, to ) // the destination email address
                    .putExtra(Intent.EXTRA_STREAM, path ) // the attachment file
                    .putExtra(Intent.EXTRA_SUBJECT, subject ); // the mail subject

            sendEmailButton.setText(
                    getResources().getStringArray( R.array.send_database_button )[language] );

            final String chooser_title = getString(R.string.send_email_chooser_title);
            sendEmailButton.setOnClickListener( new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    startActivity( Intent.createChooser( emailIntent, chooser_title ) );
                }
            });
        }
    }

    private String getUserID() {
        String ident = null;
        // Reads the file containing personal information
        File user_info_file = new File( user_info_path );
        if ( user_info_file.exists() ) {
            // The file exists, we try to read expected information
            try {
                BufferedReader reader = new BufferedReader( new FileReader(user_info_file) );
                try {
                    String line;
                    if ( (line = reader.readLine()) != null ) {
                        // Success ! 'data' holds the CSV data from the first line
                        String[] data = line.split("," );
                        // We retrieve relevant information and save that fact in 'saveInfo_gathered'
                        ident = data[1].replace( " ", "" );
                    }
                    reader.close(); // The reader is closed
                } catch ( IOException e2 ) {
                    e2.printStackTrace(); // Dealing with exceptions ..
                }
            } catch ( FileNotFoundException e ) {
                e.printStackTrace(); // Dealing with exceptions ..
            }
        }

        return ident;
    }
}
