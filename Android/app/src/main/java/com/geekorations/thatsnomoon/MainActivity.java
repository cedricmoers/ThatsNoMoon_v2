package com.geekorations.thatsnomoon;

import android.content.res.ColorStateList;
import android.content.res.Configuration;
import android.graphics.Color;
import android.graphics.LightingColorFilter;
import android.graphics.Typeface;
import android.media.MediaPlayer;
import android.support.v4.view.ViewPager;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.Gravity;
import android.view.View;
import android.view.animation.Animation;
import android.view.animation.AnimationUtils;
import android.widget.SeekBar;
import android.widget.TextSwitcher;
import android.widget.TextView;
import android.widget.Toast;
import android.widget.ViewSwitcher;

import java.util.Random;
import java.util.Timer;
import java.util.TimerTask;

import org.eclipse.paho.android.service.MqttAndroidClient;
import org.eclipse.paho.client.mqttv3.IMqttActionListener;
import org.eclipse.paho.client.mqttv3.IMqttDeliveryToken;
import org.eclipse.paho.client.mqttv3.IMqttToken;
import org.eclipse.paho.client.mqttv3.MqttCallback;
import org.eclipse.paho.client.mqttv3.MqttClient;
import org.eclipse.paho.client.mqttv3.MqttConnectOptions;
import org.eclipse.paho.client.mqttv3.MqttException;
import org.eclipse.paho.client.mqttv3.MqttMessage;

public class MainActivity extends AppCompatActivity {

    static String   MQTT_TOPICHEADER = "apps/thatsnomoon";
    static String   MQTT_SERVER = "m23.cloudmqtt.com";
    static int      MQTT_PORT = 15816;
    static String   MQTT_USER = "gaihleym";
    static String   MQTT_PASSWORD = "lRxHBxLB1z1r";
    static String   MQTT_CLIENTID = "AndroidApp_thatsnomoon";

    static String   TOPIC_RSSI = "thatsnomoon/#";

    MqttAndroidClient client;
    MediaPlayer lightsaberOnMP, lightsaberOffMP, lightsaberHumMP, backgroundMP, ewokHorn1MP, ewokHorn2MP, ewokHorn3MP, introMP;
    SeekBar seekbarOveralBrightness;
    Timer timer;

    int[] introSounds={
            R.raw.intro1,
            R.raw.intro2,
            R.raw.intro3,
            R.raw.intro4,
            R.raw.intro5,
            R.raw.intro7,
            R.raw.intro8,
            R.raw.intro9,
            R.raw.intro10,
            R.raw.intro11,
            R.raw.intro12,
            R.raw.intro13,
            R.raw.intro14,
            R.raw.intro15,
            R.raw.intro16,
            R.raw.intro17,
            R.raw.intro18,
            R.raw.intro19,
            R.raw.intro20,
            R.raw.intro21,
            R.raw.intro22,
            R.raw.intro23,
            R.raw.intro24,
    };


    boolean toggle;

    Sparkler    thrustersSparkler;
    Sparkler    stars1Sparkler;
    Sparkler    stars2Sparkler;
    Sparkler    stars3Sparkler;
    Blinker     shipLandingBlinker;
    Blinker     shipIncomingBlinker;
    Light       landingPadLight;

    boolean seekbarOveralBrightnessIsTracking;
    boolean changingSeekbar;

    TextView TextViewRSSI;
    TextView TextViewAP;

    TextSwitcher txtSwtchr;
    private ViewPager pager;

    @Override
    public void onConfigurationChanged(Configuration newConfig) {
        super.onConfigurationChanged(newConfig);
        setContentView(R.layout.activity_main);
    }

    @Override
    public void onResume(){
        super.onResume();

        backgroundMP.setLooping(true);
        backgroundMP.start();

    }

    @Override
    public void onPause() {
        super.onPause();

        backgroundMP.pause();
    }

    @Override
    public void onStop() {
        super.onStop();
    }





    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        final Typeface font = Typeface.createFromAsset(getAssets(), "fonts/Aurebesh.ttf");


        txtSwtchr = (TextSwitcher)findViewById(R.id.textSwitcher);
        txtSwtchr.setFactory(new ViewSwitcher.ViewFactory() {
            @Override
            public View makeView() {
                TextView t = new TextView(MainActivity.this);
                t.setHeight(400);
                t.setGravity(Gravity.CENTER);
                t.setTextSize(32);
                t.setTextColor(Color.WHITE);

                if (toggle == true){
                    toggle = false;
                    t.setTypeface(font, Typeface.NORMAL);
                }
                else {
                    toggle = true;
                    t.setTypeface(null, Typeface.NORMAL);
                }

                return t;
            }
        });

        // Declare the in and out animations and initialize them
        Animation in = AnimationUtils.loadAnimation(this,android.R.anim.fade_in);
        Animation out = AnimationUtils.loadAnimation(this,android.R.anim.fade_out);

        // set the animation type of textSwitcher
        txtSwtchr.setInAnimation(in);
        txtSwtchr.setOutAnimation(out);

        TextViewRSSI = (TextView)findViewById(R.id.textViewRSSI);
        TextViewAP = (TextView)findViewById(R.id.textViewWifiAP);

        this.thrustersSparkler = (Sparkler) findViewById(R.id.thrustersSparkler);
        this.stars1Sparkler = (Sparkler) findViewById(R.id.stars1Sparkler);
        this.stars2Sparkler = (Sparkler) findViewById(R.id.stars2Sparkler);
        this.stars3Sparkler = (Sparkler) findViewById(R.id.stars3Sparkler);
        this.shipIncomingBlinker = (Blinker) findViewById(R.id.incomingShipBlinker);
        this.shipLandingBlinker = (Blinker) findViewById(R.id.landingShipBlinker);
        this.landingPadLight = (Light) findViewById(R.id.landingPlatformSpots);

        String clientId = MqttClient.generateClientId();

        client = new MqttAndroidClient(this.getApplicationContext(), "tcp://m23.cloudmqtt.com:15816" , clientId);

        MqttConnectOptions options = new MqttConnectOptions();

        options.setUserName(MQTT_USER);
        options.setPassword(MQTT_PASSWORD.toCharArray());


        try {
            IMqttToken token = client.connect(options);

            token.setActionCallback(new IMqttActionListener() {
                @Override
                public void onSuccess(IMqttToken asyncActionToken) {
                    Toast.makeText(MainActivity.this, "Connected.", Toast.LENGTH_LONG).show();
                    setSubscriptions();

                }

                @Override
                public void onFailure(IMqttToken asyncActionToken, Throwable exception) {
                    Toast.makeText(MainActivity.this, "Connection failed.", Toast.LENGTH_LONG).show();
                }
            });
        } catch (MqttException e) {
            e.printStackTrace();
        }

        client.setCallback(new MqttCallback() {
            @Override
            public void connectionLost(Throwable cause) {

            }

            @Override
            public void messageArrived(String topic, MqttMessage message) throws Exception {

                if (topic.equals("thatsnomoon/wifi/rssi")) {
                    TextViewRSSI.setText(new String(message.getPayload()));
                }
                else if (topic.equals("thatsnomoon/wifi/ap")) {
                    TextViewAP.setText(new String(message.getPayload()));
                }
                else if (topic.startsWith("thatsnomoon/incomingthrusters")) {
                    thrustersSparkler.processMessage(topic, message);
                }
                else if (topic.startsWith("thatsnomoon/stars1")) {
                    stars1Sparkler.processMessage(topic, message);
                }
                else if (topic.startsWith("thatsnomoon/stars2")) {
                    stars2Sparkler.processMessage(topic, message);
                }
                else if (topic.startsWith("thatsnomoon/stars3")) {
                    stars3Sparkler.processMessage(topic, message);
                }
                else if (topic.startsWith("thatsnomoon/incomingstrobes")) {
                    shipIncomingBlinker.processMessage(topic, message);
                }
                else if (topic.startsWith("thatsnomoon/landingstrobes")) {
                    shipLandingBlinker.processMessage(topic, message);
                }
                else if (topic.startsWith("thatsnomoon/platformspots")) {
                    shipLandingBlinker.processMessage(topic, message);
                }
            }

            @Override
            public void deliveryComplete(IMqttDeliveryToken token) {

            }
        });

        int randomIntroSound = this.introSounds[new Random().nextInt(introSounds.length)];

        backgroundMP    = MediaPlayer.create(this, R.raw.endor_backgroundsounds);
        introMP         = MediaPlayer.create(this, randomIntroSound);
        ewokHorn1MP     = MediaPlayer.create(this, R.raw.ewok_horn_var1);
        ewokHorn2MP     = MediaPlayer.create(this, R.raw.ewok_horn_var2);
        ewokHorn3MP     = MediaPlayer.create(this, R.raw.ewok_horn_var3);

        lightsaberOnMP  = MediaPlayer.create(this, R.raw.lightsabre_on);
        lightsaberOffMP = MediaPlayer.create(this, R.raw.lightsabre_off);
        lightsaberHumMP = MediaPlayer.create(this, R.raw.lightsabre_hum);
        lightsaberHumMP.setLooping(true);

        introMP.start();

        seekbarOveralBrightnessIsTracking = false;
        seekbarOveralBrightness = (SeekBar) findViewById(R.id.seekbarOveralBrightness);
        seekbarOveralBrightness.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {
                seekbarOveralBrightnessIsTracking = false;
                publish("apps/thatsnomoon/brightness", String.valueOf(seekbarOveralBrightness.getProgress()));
                lightsaberOffMP.start();
                lightsaberHumMP.pause();

            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {
                seekbarOveralBrightnessIsTracking = true;
                publish("apps/thatsnomoon/brightness", String.valueOf(seekbarOveralBrightness.getProgress()));
                lightsaberOnMP.start();
                lightsaberHumMP.setLooping(true);
                lightsaberHumMP.start();
            }

            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
            }
        });

        timer = new Timer();

        timer.scheduleAtFixedRate(new TimerTask() {
            @Override
            public void run() {
                if (seekbarOveralBrightnessIsTracking == true) {
                    publish("apps/thatsnomoon/brightness", String.valueOf(seekbarOveralBrightness.getProgress()));
                }
            }
        }, 0, 500);

        changingSeekbar = false;

        this.thrustersSparkler.setMQTTPublishTopic("apps/thatsnomoon/incomingthrusters");
        this.thrustersSparkler.setMQTTReceiveTopic("thatsnomoon/incomingthrusters");
        this.thrustersSparkler.setMQTTClient(client);
        this.thrustersSparkler.setNextCardButtonClickedListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                onNextCard();
            }
        });
        this.thrustersSparkler.setPreviousCardButtonClickedListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                onPreviousCard();
            }
        });

        this.stars1Sparkler.setMQTTPublishTopic("apps/thatsnomoon/stars1");
        this.stars1Sparkler.setMQTTReceiveTopic("thatsnomoon/stars1");
        this.stars1Sparkler.setMQTTClient(client);
        this.stars1Sparkler.setNextCardButtonClickedListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                onNextCard();
            }
        });
        this.stars1Sparkler.setPreviousCardButtonClickedListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                onPreviousCard();
            }
        });

        this.stars2Sparkler.setMQTTPublishTopic("apps/thatsnomoon/stars2");
        this.stars2Sparkler.setMQTTReceiveTopic("thatsnomoon/stars2");
        this.stars2Sparkler.setMQTTClient(client);
        this.stars2Sparkler.setNextCardButtonClickedListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                onNextCard();
            }
        });
        this.stars2Sparkler.setPreviousCardButtonClickedListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                onPreviousCard();
            }
        });

        this.stars3Sparkler.setMQTTPublishTopic("apps/thatsnomoon/stars3");
        this.stars3Sparkler.setMQTTReceiveTopic("thatsnomoon/stars3");
        this.stars3Sparkler.setMQTTClient(client);
        this.stars3Sparkler.setNextCardButtonClickedListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                onNextCard();
            }
        });
        this.stars3Sparkler.setPreviousCardButtonClickedListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                onPreviousCard();
            }
        });

        this.shipIncomingBlinker.setMQTTPublishTopic("apps/thatsnomoon/incomingstrobes");
        this.shipIncomingBlinker.setMQTTReceiveTopic("thatsnomoon/incomingstrobes");
        this.shipIncomingBlinker.setMQTTClient(client);
        this.shipIncomingBlinker.setNextCardButtonClickedListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                onNextCard();
            }
        });
        this.shipIncomingBlinker.setPreviousCardButtonClickedListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                onPreviousCard();
            }
        });

        this.shipLandingBlinker.setMQTTPublishTopic("apps/thatsnomoon/landingstrobes");
        this.shipLandingBlinker.setMQTTReceiveTopic("thatsnomoon/landingstrobes");
        this.shipLandingBlinker.setMQTTClient(client);
        this.shipLandingBlinker.setNextCardButtonClickedListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                onNextCard();
            }
        });
        this.shipLandingBlinker.setPreviousCardButtonClickedListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                onPreviousCard();
            }
        });

        this.landingPadLight.setMQTTPublishTopic("apps/thatsnomoon/platformspots");
        this.landingPadLight.setMQTTReceiveTopic("thatsnomoon/platformspots");
        this.landingPadLight.setMQTTClient(client);
        this.landingPadLight.setNextCardButtonClickedListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                onNextCard();
            }
        });
        this.landingPadLight.setPreviousCardButtonClickedListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                onPreviousCard();
            }
        });


        CardPagerAdapter adapter = new CardPagerAdapter();
        this.pager = (ViewPager) findViewById(R.id.pager);
        this.pager.setOffscreenPageLimit(10);
        this.pager.setAdapter(adapter);


    }

    public void publishModeDay(View v) {
        ewokHorn1MP.start();
        txtSwtchr.setText("DAY");
        publish("apps/thatsnomoon/mode", "day");
    }

    public void publishModeNight(View v) {
        ewokHorn3MP.start();
        txtSwtchr.setText("NIGHT");
        publish("apps/thatsnomoon/mode", "night");
    }

    public void publishModeOff(View v) {
        ewokHorn2MP.start();
        txtSwtchr.setText("OFF");
        publish("apps/thatsnomoon/mode", "off");
    }

    public void publish(String topic, String message){

        try {
            client.publish(topic, message.getBytes(), 0, false);
        } catch (MqttException e) {
            e.printStackTrace();
        }
    }

    private void setSubscriptions(){
        try{
            client.subscribe(TOPIC_RSSI, 0);
        }
        catch(MqttException e) {
            e.printStackTrace();
        }
    }

    private void onNextCard() {
        this.pager.setCurrentItem(getCurrentPagerItem(+1), true);
    }

    private void onPreviousCard() {
        this.pager.setCurrentItem(getCurrentPagerItem(-1), true);
    }

    private int getCurrentPagerItem(int offset) {
        return this.pager.getCurrentItem() + offset;
    }
}
