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

public class Light extends LedCard {

    public Parameter brightnessParameter;

    private int maxBrightness;
    private int startBrightness;

    public Light(Context context) {
        super(context);

        initializeViews(context);
    }

    public Light(Context context, AttributeSet attrs) {
        super(context, attrs);

        TypedArray a = context.obtainStyledAttributes(attrs, R.styleable.Light);

        maxBrightness       = a.getInteger(R.styleable.Light_maxBrightness, 1023);

        startBrightness     = a.getInteger(R.styleable.Light_startBrightness, maxBrightness);

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

        brightnessParameter     = new Parameter(this.getContext());

        this.brightnessParameter.setBarMax(maxBrightness);
        this.brightnessParameter.setName("Brightness");
        this.brightnessParameter.setValue(startBrightness);

        Parameter[] parameters = new Parameter[] {
                brightnessParameter
        };

        this.setParameters(parameters);

    }


    public void setMQTTPublishTopic(String topic) {

        this.mqttPublishTopic = topic;

        this.brightnessParameter.setMQTTTopic(this.mqttPublishTopic + "/" +"brightness");

    }

    public void processMessage(String topic, MqttMessage message) {

        if (topic.equals(this.mqttReceiveTopic + "/" +"brightness")) {
            this.brightnessParameter.setValue(Integer.parseInt(new String(message.getPayload())));
        }
        else {
            //do nothing.
        }
    }
}
