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

public class Blinker extends LedCard {

    public Parameter brightnessParameter;
    public Parameter onTimeParameter;
    public Parameter offTimeParameter;
    public Parameter offsetTimeParameter;

    private int maxBrightness;
    private int maxOnTime;
    private int maxOffTime;
    private int maxOffsetTime;

    private int startBrightness;
    private int startOnTime;
    private int startOffTime;
    private int startOffsetTime;

    public Blinker(Context context) {
        super(context);
        initializeViews(context);
    }

    public Blinker(Context context, AttributeSet attrs) {
        super(context, attrs);

        TypedArray a = context.obtainStyledAttributes(attrs, R.styleable.Blinker);

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
        inflater.inflate(R.layout.ledcard_view, this);
    }

    protected void onFinishInflate() {
        super.onFinishInflate();

        this.brightnessParameter     = new Parameter(this.getContext());
        this.onTimeParameter         = new Parameter(this.getContext());
        this.offTimeParameter        = new Parameter(this.getContext());
        this.offsetTimeParameter     = new Parameter(this.getContext());

        this.brightnessParameter.setBarMax(maxBrightness);
        this.onTimeParameter.setBarMax(maxOnTime);
        this.offTimeParameter.setBarMax(maxOffTime);
        this.offsetTimeParameter.setBarMax(maxOffsetTime);

        this.brightnessParameter.setValue(startBrightness);
        this.onTimeParameter.setValue(startOnTime);
        this.offTimeParameter.setValue(startOffTime);
        this.offsetTimeParameter.setValue(startOffsetTime);

        this.brightnessParameter.setName("Brightness");
        this.onTimeParameter.setName("On Time");
        this.offTimeParameter.setName("Off Time");
        this.offsetTimeParameter.setName("Offset");

        Parameter[] parameters = new Parameter[] {
                this.brightnessParameter,
                this.onTimeParameter,
                this.offTimeParameter,
                this.offsetTimeParameter
        };

        this.setParameters(parameters);
    }

    public void setMQTTPublishTopic(String topic) {

        this.mqttPublishTopic = topic;

        this.brightnessParameter.setMQTTTopic(this.mqttPublishTopic + "/" +"brightness");
        this.onTimeParameter.setMQTTTopic(this.mqttPublishTopic + "/" +"ontime");
        this.offTimeParameter.setMQTTTopic(this.mqttPublishTopic + "/" +"offtime");
        this.offsetTimeParameter.setMQTTTopic(this.mqttPublishTopic + "/" +"offsettime");

    }

    public void processMessage(String topic, MqttMessage message) {

        if (topic.equals(this.mqttReceiveTopic + "/" +"brightness")) {
            this.brightnessParameter.setValue(Integer.parseInt(new String(message.getPayload())));
        }
        else if (topic.equals(this.mqttReceiveTopic + "/" + "ontime")) {
            this.onTimeParameter.setValue(Integer.parseInt(new String(message.getPayload())));
        }
        else if (topic.equals(this.mqttReceiveTopic + "/" + "offtime")) {
            this.offTimeParameter.setValue(Integer.parseInt(new String(message.getPayload())));
        }
        else if (topic.equals(this.mqttReceiveTopic + "/" + "offsettime")) {
            this.offsetTimeParameter.setValue(Integer.parseInt(new String(message.getPayload())));
        }
        else {
            //do nothing.
        }
    }
}
