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

public class Light extends LinearLayout {

    private String MQTTPublishTopic;
    private String MQTTReceiveTopic;

    public Parameter brightnessParameter;

    private String lightName;
    private int maxBrightness;

    private int startBrightness;

    private TextView nameTextView;
    private TextView foreignNameTextView;

    public Light(Context context) {
        super(context);

        initializeViews(context);
    }

    public Light(Context context, AttributeSet attrs) {
        super(context, attrs);

        TypedArray a = context.obtainStyledAttributes(attrs, R.styleable.Light);

        lightName           = a.getString(R.styleable.Light_lightName);

        maxBrightness       = a.getInteger(R.styleable.Light_maxBrightness, 1023);

        startBrightness     = a.getInteger(R.styleable.Light_startBrightness, maxBrightness);

        a.recycle();

        initializeViews(context);
    }

    private void initializeViews(Context context) {
        LayoutInflater inflater = (LayoutInflater) context
                .getSystemService(Context.LAYOUT_INFLATER_SERVICE);
        inflater.inflate(R.layout.light_view, this);
    }

    protected void onFinishInflate() {
        super.onFinishInflate();

        brightnessParameter     = (Parameter) this.findViewById(R.id.brightnessParameter);

        nameTextView            = (TextView) this.findViewById(R.id.nameTextView);
        foreignNameTextView     = (TextView) this.findViewById(R.id.foreignNameTextView);

        this.brightnessParameter.setBarMax(maxBrightness);

        this.brightnessParameter.setBarValue(startBrightness);

        Typeface font = Typeface.createFromAsset(getContext().getAssets(), "fonts/Aurebesh.ttf");
        foreignNameTextView.setTypeface(font, Typeface.NORMAL);

        foreignNameTextView.setSelected(true);

        setLightName(lightName);
    }

    public void setLightName(String name) {

        lightName = name;
        nameTextView.setText(name);
        foreignNameTextView.setText(name);

    }

    public void setMQTTClient(MqttAndroidClient client) {

        this.brightnessParameter.setMQTTClient(client);

    }

    public void setMQTTPublishTopic(String topic) {

        this.MQTTPublishTopic = topic;

        this.brightnessParameter.setMQTTTopic(this.MQTTPublishTopic + "/" +"brightness");

    }

    public void setMQTTReceiveTopic(String topic) {

        this.MQTTReceiveTopic = topic;

    }

    public void processMessage(String topic, MqttMessage message) {

        if (topic.equals(this.MQTTReceiveTopic + "/" +"brightness")) {
            this.brightnessParameter.setActualValue(new String(message.getPayload()));
            this.brightnessParameter.setBarValue(Integer.parseInt(new String(message.getPayload())));
        }
        else {
            //do nothing.
        }
    }
}
