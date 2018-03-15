package com.geekorations.thatsnomoon;

import android.content.Context;
import android.content.res.TypedArray;
import android.graphics.Typeface;
import android.util.AttributeSet;
import android.view.LayoutInflater;
import android.widget.LinearLayout;
import android.widget.TextView;

import org.eclipse.paho.android.service.MqttAndroidClient;
import org.eclipse.paho.client.mqttv3.MqttMessage;

/**
 * Created by Cedric on 11/02/2018.
 */

public class Blinker extends LinearLayout {

    private String MQTTPublishTopic;
    private String MQTTReceiveTopic;

    public Parameter brightnessParameter;
    public Parameter onTimeParameter;
    public Parameter offTimeParameter;
    public Parameter offsetTimeParameter;

    private String blinkerName;
    private int maxBrightness;
    private int maxOnTime;
    private int maxOffTime;
    private int maxOffsetTime;

    private int startBrightness;
    private int startOnTime;
    private int startOffTime;
    private int startOffsetTime;

    private TextView nameTextView;
    private TextView foreignNameTextView;

    public Blinker(Context context) {
        super(context);

        initializeViews(context);
    }

    public Blinker(Context context, AttributeSet attrs) {
        super(context, attrs);

        TypedArray a = context.obtainStyledAttributes(attrs, R.styleable.Blinker);

        blinkerName = a.getString(R.styleable.Blinker_blinkerName);

        maxBrightness       = a.getInteger(R.styleable.Blinker_maxBrightness, 1023);
        maxOnTime           = a.getInteger(R.styleable.Blinker_maxOnTime, 5000);
        maxOffTime          = a.getInteger(R.styleable.Blinker_maxOffTime, 5000);
        maxOffsetTime       = a.getInteger(R.styleable.Blinker_maxOffsetTime, 1000);

        startBrightness     = a.getInteger(R.styleable.Blinker_startBrightness, maxBrightness);
        startOnTime         = a.getInteger(R.styleable.Blinker_startOnTime, 200);
        startOffTime        = a.getInteger(R.styleable.Blinker_startOffTime, 800);
        startOffsetTime     = a.getInteger(R.styleable.Blinker_startOffsetTime, 0);

        a.recycle();

        initializeViews(context);
    }

    private void initializeViews(Context context) {
        LayoutInflater inflater = (LayoutInflater) context
                .getSystemService(Context.LAYOUT_INFLATER_SERVICE);
        inflater.inflate(R.layout.blinker_view, this);
    }

    protected void onFinishInflate() {
        super.onFinishInflate();

        brightnessParameter     = (Parameter) this.findViewById(R.id.brightnessParameter);
        onTimeParameter         = (Parameter) this.findViewById(R.id.onTimeParameter);
        offTimeParameter        = (Parameter) this.findViewById(R.id.offTimeParameter);
        offsetTimeParameter     = (Parameter) this.findViewById(R.id.offsetTimeParameter);

        nameTextView            = (TextView) this.findViewById(R.id.nameTextView);
        foreignNameTextView     = (TextView) this.findViewById(R.id.foreignNameTextView);

        this.brightnessParameter.setBarMax(maxBrightness);
        this.onTimeParameter.setBarMax(maxOnTime);
        this.offTimeParameter.setBarMax(maxOffTime);
        this.offsetTimeParameter.setBarMax(maxOffsetTime);

        this.brightnessParameter.setBarValue(startBrightness);
        this.onTimeParameter.setBarValue(startOnTime);
        this.offTimeParameter.setBarValue(startOffTime);
        this.offsetTimeParameter.setBarValue(startOffsetTime);

        Typeface font = Typeface.createFromAsset(getContext().getAssets(), "fonts/Aurebesh.ttf");
        foreignNameTextView.setTypeface(font, Typeface.NORMAL);

        foreignNameTextView.setSelected(true);

        setBlinkerName(blinkerName);
    }

    public void setBlinkerName(String name) {

        blinkerName = name;
        nameTextView.setText(name);
        foreignNameTextView.setText(name);

    }

    public void setMQTTClient(MqttAndroidClient client) {

        this.brightnessParameter.setMQTTClient(client);
        this.onTimeParameter.setMQTTClient(client);
        this.offTimeParameter.setMQTTClient(client);
        this.offsetTimeParameter.setMQTTClient(client);

    }

    public void setMQTTPublishTopic(String topic) {

        this.MQTTPublishTopic = topic;

        this.brightnessParameter.setMQTTTopic(this.MQTTPublishTopic + "/" +"brightness");
        this.onTimeParameter.setMQTTTopic(this.MQTTPublishTopic + "/" +"ontime");
        this.offTimeParameter.setMQTTTopic(this.MQTTPublishTopic + "/" +"offtime");
        this.offsetTimeParameter.setMQTTTopic(this.MQTTPublishTopic + "/" +"offsettime");

    }

    public void setMQTTReceiveTopic(String topic) {

        this.MQTTReceiveTopic = topic;

    }

    public void processMessage(String topic, MqttMessage message) {

        if (topic.equals(this.MQTTReceiveTopic + "/" +"brightness")) {
            this.brightnessParameter.setActualValue(new String(message.getPayload()));
            this.brightnessParameter.setBarValue(Integer.parseInt(new String(message.getPayload())));
        }
        else if (topic.equals(this.MQTTReceiveTopic + "/" + "ontime")) {
            this.onTimeParameter.setActualValue(new String(message.getPayload()));
            this.onTimeParameter.setBarValue(Integer.parseInt(new String(message.getPayload())));
        }
        else if (topic.equals(this.MQTTReceiveTopic + "/" + "offtime")) {
            this.offTimeParameter.setActualValue(new String(message.getPayload()));
            this.offTimeParameter.setBarValue(Integer.parseInt(new String(message.getPayload())));
        }
        else if (topic.equals(this.MQTTReceiveTopic + "/" + "offsettime")) {
            this.offsetTimeParameter.setActualValue(new String(message.getPayload()));
            this.offsetTimeParameter.setBarValue(Integer.parseInt(new String(message.getPayload())));
        }
        else {
            //do nothing.
        }
    }
}
