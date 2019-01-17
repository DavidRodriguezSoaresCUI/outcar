package org.libsdl.app;

import android.app.Activity;
import android.content.res.Resources;
import android.os.*;
import android.content.Intent;
import android.view.View;
import android.widget.*;
import java.io.*;
import java.util.Locale;

import com.outcar.game.R;

public class Tutorial extends Activity {
    private int language, state, maxState, skippable;
    private int ptsBonusAvoid, ptsBonusRefuel, ptsPenaltyCrash, ptsPenaltyNoFuel;
    private boolean error, need_to_refuel, display_pause_button;
    private View tutorial_view;
    private TextView tutorial_text;
    private ImageView tutorial_text_background;
    private Button end_tutorial_button, touch_to_continue, touch_go_back;
    private CheckBox end_tutorial_checkBox;
    private Resources Res;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_tutorial);

        language = getIntent().getIntExtra( "language", 0 );
        skippable = getIntent().getIntExtra( "skippable", 0 );

        tutorial_view            = (View)      findViewById( R.id.tutorial_view );
        tutorial_text_background = (ImageView) findViewById( R.id.tutoriel_text_img );
        tutorial_text            = (TextView)  findViewById( R.id.tutorial_text );
        end_tutorial_button      = (Button)    findViewById( R.id.end_tutorial_button );
        end_tutorial_checkBox    = (CheckBox)  findViewById( R.id.end_tutorial_checkBox );
        touch_to_continue        = (Button)    findViewById( R.id.touch_to_continue_imageButton );
        touch_go_back            = (Button)    findViewById( R.id.touch_to_go_back_imageButton );
        Res = getResources();
        state = 0;
        maxState = Res.getInteger( R.integer.tutorial_pages );

        // Adds padding to the bottom, moving text higher on screen
        tutorial_text.setPadding(
                tutorial_text.getPaddingLeft(),
                tutorial_text.getPaddingTop(),
                tutorial_text.getPaddingRight(),
                tutorial_text.getPaddingBottom() + 100);
        tutorial_text_background.setPadding(
                tutorial_text_background.getPaddingLeft(),
                tutorial_text_background.getPaddingTop(),
                tutorial_text_background.getPaddingRight(),
                tutorial_text_background.getPaddingBottom() + 100);

        ptsBonusAvoid        = 0;
        ptsBonusRefuel       = 0;
        ptsPenaltyCrash      = 0;
        ptsPenaltyNoFuel     = 0;
        need_to_refuel       = false;
        display_pause_button = false;
        error                = false;

        String pers_info_file_folder = Environment.getExternalStorageDirectory().toString()
                + File.separator + getString( R.string.app_name );
        String game_config_file =  pers_info_file_folder
                + File.separator + getString(R.string.game_config_file);

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

        try {
            File file = new File(game_config_file);
            String line = null;

            // Reads configuration info from file to retrieve point bonuses/penalties
            if ( file.exists() ) {
                BufferedReader csvReader = new BufferedReader(new FileReader( file ));

                while( (line = csvReader.readLine()) != null )
                {
                    if( !line.contains( "//" ) )
                        break;
                }

                csvReader.close();
            }

            if( line == null ) // if the file or the line wasn't found, we load the defaults
            {
                line = Res.getString( R.string.game_config_file_default_values );
            }

            String[] data = line.split( "," );
            int ptsIndex = Res.getInteger( R.integer.pts_index );
            if( data.length != Res.getInteger( R.integer.config_items ) )
            {
                error = true;
            }
            else
            {
                need_to_refuel = (Integer.valueOf(
                        data[ptsIndex-2].replace( " ", "" ) ) != 0);
                display_pause_button = (Integer.valueOf(
                        data[ptsIndex-1].replace( " ", "" ) ) != 0);
                ptsBonusAvoid    = Integer.valueOf(
                        data[ ptsIndex ].replace( " ", "" ) );
                ptsBonusRefuel   = Integer.valueOf(
                        data[ptsIndex+1].replace( " ", "" ) );
                ptsPenaltyCrash  = Integer.valueOf(
                        data[ptsIndex+2].replace( " ", "" ) );
                ptsPenaltyNoFuel = Integer.valueOf(
                        data[ptsIndex+3].replace( " ", "" ) );
            }
        } catch (Exception e) {
            e.printStackTrace();
        }

        update_tutorial_page();
    }

    private void update_tutorial_page()
    {
        /* Pages:
         * 0:Welcome
         * 1:Point system
         * 2:Interface
         * 3:Pause
         * 4:Steering
         * 5-6:Show fuel
         * 7:Refuel
         * 8:Incentivise refueling
         * 9:End of Tutorial
         *
         */
        switch (state){
            case 0:
                setTutorialText();
                tutorial_view.setBackgroundResource( R.drawable.bkgrd_tuto_00 );
                touch_to_continue.setText( Res.getStringArray( R.array.touch2continue )[language] );
                tutorial_text_background.setVisibility(View.GONE);
                end_tutorial_checkBox.setVisibility(View.GONE);
                end_tutorial_button.setVisibility(View.GONE);
                touch_go_back.setVisibility(View.GONE);
                break;
            case 1:
                setTutorialText_forScores();
                tutorial_view.setBackgroundResource( R.drawable.bkgrd_tuto_00 );
                touch_go_back.setText( Res.getStringArray( R.array.touch2goBack )[language] );
                touch_go_back.setVisibility(View.VISIBLE);
                touch_to_continue.setVisibility( View.VISIBLE );
                tutorial_text_background.setVisibility(View.GONE);
                break;
            case 2:
                setTutorialText();
                tutorial_view.setBackgroundResource( R.drawable.bkgrd_tuto_01_v3 );
                touch_to_continue.setVisibility( View.GONE );
                tutorial_text_background.setVisibility(View.VISIBLE);
                touch_go_back.setText( "" );
                break;
            case 3:
                setTutorialText();
                tutorial_view.setBackgroundResource( R.drawable.bkgrd_tuto_02_v3 );
                break;
            case 4:
                setTutorialText();
                tutorial_view.setBackgroundResource( R.drawable.bkgrd_tuto_03_v3 );
                break;
            case 5:
                setTutorialText();
                tutorial_view.setBackgroundResource( R.drawable.bkgrd_tuto_04_v3 );
                break;
            case 6:
                setTutorialText();
                tutorial_view.setBackgroundResource( R.drawable.bkgrd_tuto_05_v3 );
                break;
            case 7:
                setTutorialText();
                tutorial_view.setBackgroundResource( R.drawable.bkgrd_tuto_06_v3 );
                break;
            case 8:
                setTutorialText();
                tutorial_view.setBackgroundResource( R.drawable.bkgrd_tuto_07_v3 );
                tutorial_text_background.setVisibility(View.VISIBLE);
                end_tutorial_checkBox.setVisibility(View.GONE);
                end_tutorial_button.setVisibility(View.GONE);
                break;
            case 9:
                setTutorialText();
                tutorial_view.setBackgroundResource( R.drawable.bkgrd_tuto_00 );
                tutorial_text_background.setVisibility(View.GONE);
                touch_go_back.setVisibility(View.GONE);
                end_tutorial_checkBox.setText( Res.getStringArray(
                        R.array.end_tutorial_checkBox )[language] );
                end_tutorial_checkBox.setVisibility(View.VISIBLE);
                end_tutorial_button.setText( Res.getStringArray(
                        R.array.end_tutorial_button_unchecked )[language] );
                end_tutorial_button.setVisibility(View.VISIBLE);
                break;
            default:
                tutorial_text.setText( R.string.std_error_msg );
        }
    }

    private void setTutorialText()
    {
        int txt_ident = Res.getIdentifier( "instr_cruiser_"
                + String.valueOf( state ), "array", Tutorial.this.getPackageName() );

        if (txt_ident != 0)
            tutorial_text.setText( Res.getStringArray( txt_ident )[language] );
        else
            Toast.makeText(Tutorial.this, R.string.std_error_msg, Toast.LENGTH_SHORT).show();
    }

    private void setTutorialText_forScores()
    {
        // We get the ident for all supported formats
        int txt_ident = Res.getIdentifier(
                "instr_cruiser_" + String.valueOf( state ),
                "array", Tutorial.this.getPackageName() );
        int txt_ident_no_oof_penalty = Res.getIdentifier(
                "instr_cruiser_" + String.valueOf( state ) + "_no_oof_penalty",
                "array", Tutorial.this.getPackageName() );
        int txt_ident_no_refuel_mechanic = Res.getIdentifier(
                "instr_cruiser_" + String.valueOf( state ) + "_no_refuel_mechanic",
                "array", Tutorial.this.getPackageName() );

        if ( txt_ident == 0 || txt_ident_no_oof_penalty == 0 || txt_ident_no_refuel_mechanic == 0 )
        {
            Toast.makeText(Tutorial.this, R.string.std_error_msg, Toast.LENGTH_SHORT).show();
            return;
        }

        if( error ) // an error occurred, we set a default error message as text
        {
            tutorial_text.setText( Res.getString( R.string.std_error_msg ) );
            return;
        }

        // We get the base text and then the placeholders are replaced by the values
        String baseText = null;
        if ( !need_to_refuel )
        {
            baseText = Res.getStringArray( txt_ident_no_refuel_mechanic )[language];
        }
        else if ( ptsPenaltyNoFuel == 0 )
        {
            baseText = Res.getStringArray( txt_ident_no_oof_penalty )[language];
        }
        else
        {
            baseText = Res.getStringArray( txt_ident )[language];
        }

        if ( baseText == null )
        {
            Toast.makeText(Tutorial.this, R.string.std_error_msg, Toast.LENGTH_SHORT).show();
            return;
        }

        String formatedText;
        if ( !need_to_refuel )
        {
            formatedText = String.format( Locale.US, baseText,
                    ptsBonusAvoid,
                    ptsPenaltyCrash );
        }
        else if ( ptsPenaltyNoFuel == 0 )
        {
            formatedText = String.format( Locale.US, baseText,
                    ptsBonusAvoid, ptsBonusRefuel,
                    ptsPenaltyCrash );
        }
        else
        {
            formatedText = String.format( Locale.US, baseText,
                    ptsBonusAvoid, ptsBonusRefuel,
                    ptsPenaltyCrash, ptsPenaltyNoFuel );
        }

        tutorial_text.setText( formatedText );
    }

    public void onTutorialClicked( View v ) {
        if ( state + 1 < maxState ) {
            state = changePage( state, +1 );
            update_tutorial_page();
        }
    }

    public void onTutorialGoBackClicked( View v ) {
        if ( 0 <= state - 1 ) {
            state = changePage( state, -1 );
            update_tutorial_page();
            Toast.makeText(Tutorial.this, "State:"+String.valueOf(state), Toast.LENGTH_SHORT).show();
        }
    }

    private int changePage( int previousPage, int change )
    {
        int newpage = previousPage + change;

        if ( newpage == 3 && !display_pause_button )
        {
            newpage += change;
        }
        if ( (newpage == 5 || newpage == 8) && !need_to_refuel )
        {
            newpage += 4 * change;
        }

        return newpage;
    }

    public void onEnd_tutorial_checkBox_Clicked( View v ) {
        if ( end_tutorial_checkBox.isChecked() ) {
            end_tutorial_button.setText( Res.getStringArray(
                    R.array.end_tutorial_button_checked )[language] );
        } else {
            end_tutorial_button.setText( Res.getStringArray(
                    R.array.end_tutorial_button_unchecked )[language] );
        }
    }

    public void onEnd_tutorial_button_Clicked( View v ) {
        if ( end_tutorial_checkBox.isChecked() ) {
            gotoMainActivity();
        } else {
            Tutorial.this.recreate();
        }
    }

    private void gotoMainActivity() {

        startActivity( new Intent( Tutorial.this, MainActivity.class )
                .addFlags( Intent.FLAG_ACTIVITY_NO_ANIMATION ) );
        finish();
    }

    @Override
    public void onBackPressed() {
        // If the 'back' button is pressed, we go back to the menu
        if ( skippable == 1 )
            gotoMainActivity();
        else
            Toast.makeText( Tutorial.this,
                    Res.getString( R.string.cant_go_back ), Toast.LENGTH_LONG ).show();
    }
}
