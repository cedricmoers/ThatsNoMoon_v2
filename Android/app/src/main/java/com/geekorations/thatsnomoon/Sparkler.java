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

public class Sparkler extends LinearLayout {

    private String MQTTPublishTopic;

    private String MQTTReceiveTopic;


    public Parameter brightnessParameter;
    public Parameter variationParameter;
    public Parameter smoothingParameter;

    private String sparklerName;
    private int maxBrightness;
    private int maxVariation;
    private int maxSmoothing;

    private int startBrightness;
    private int startVariation;
    private int startSmoothing;

    private TextView nameTextView;
    private TextView foreignNameTextView;


    public  Sparkler(Context context) {
        super(context);

        initializeViews(context);
    }

    public Sparkler(Context context, AttributeSet attrs) {
        super(context, attrs);

        TypedArray a = context.obtainStyledAttributes(attrs, R.styleable.Sparkler);

        sparklerName = a.getString(R.styleable.Sparkler_sparklerName);

        maxBrightness = a.getInteger(R.styleable.Sparkler_maxBrightness, 1023);
        maxVariation = a.getInteger(R.styleable.Sparkler_maxVariation, 511);
        maxSmoothing = a.getInteger(R.styleable.Sparkler_maxSmoothing, 1000);

        startBrightness = a.getInteger(R.styleable.Sparkler_startBrightness, maxBrightness);
        startVariation = a.getInteger(R.styleable.Sparkler_startVariation, maxVariation);
        startSmoothing = a.getInteger(R.styleable.Sparkler_startSmoothing, maxSmoothing);

        a.recycle();

        initializeViews(context);
    }

    private void initializeViews(Context context) {
        LayoutInflater inflater = (LayoutInflater) context
                .getSystemService(Context.LAYOUT_INFLATER_SERVICE);
        inflater.inflate(R.layout.sparkler_view, this);
    }

    protected void onFinishInflate() {
        super.onFinishInflate();

        brightnessParameter     = (Parameter) this.findViewById(R.id.brightnessParameter);
        variationParameter      = (Parameter) this.findViewById(R.id.variationParameter);
        smoothingParameter      = (Parameter) this.findViewById(R.id.smoothingParameter);

        nameTextView            = (TextView) this.findViewById(R.id.nameTextView);
        foreignNameTextView     = (TextView) this.findViewById(R.id.foreignNameTextView);

        this.brightnessParameter.setBarMax(maxBrightness);
        this.variationParameter.setBarMax(maxVariation);
        this.smoothingParameter.setBarMax(maxSmoothing);

        this.brightnessParameter.setBarValue(startBrightness);
        this.variationParameter.setBarValue(startVariation);
        this.smoothingParameter.setBarValue(startSmoothing);

        this.smoothingParameter.setValueConverter(new ExponentialValueConverter(1,-1,2,-1.0/125.0));

        Typeface font = Typeface.createFromAsset(getContext().getAssets(), "fonts/Aurebesh.ttf");
        foreignNameTextView.setTypeface(font, Typeface.NORMAL);

        setSparklerName(sparklerName);
    }

    public void setSparklerName(String name) {

        sparklerName = name;
        nameTextView.setText(name);
        foreignNameTextView.setText(name);

    }

    public void setMQTTClient(MqttAndroidClient client) {

        this.brightnessParameter.setMQTTClient(client);
        this.variationParameter.setMQTTClient(client);
        this.smoothingParameter.setMQTTClient(client);

    }

    public void setMQTTPublishTopic(String topic) {

        this.MQTTPublishTopic = topic;

        this.brightnessParameter.setMQTTTopic(this.MQTTPublishTopic + "/" +"brightness");
        this.variationParameter.setMQTTTopic(this.MQTTPublishTopic + "/" +"variation");
        this.smoothingParameter.setMQTTTopic(this.MQTTPublishTopic + "/" +"smoothing");

    }

    public void setMQTTReceiveTopic(String topic) {

        this.MQTTReceiveTopic = topic;

    }

    public void processMessage(String topic, MqttMessage message) {

        if (topic.equals(this.MQTTReceiveTopic + "/" +"brightness")) {
            this.brightnessParameter.setValue(Float.parseFloat(new String(message.getPayload())));
        }
        else if (topic.equals(this.MQTTReceiveTopic + "/" + "variation")) {
            this.variationParameter.setValue(Float.parseFloat(new String(message.getPayload())));
        }
        else if (topic.equals(this.MQTTReceiveTopic + "/" + "smoothing")) {
            this.smoothingParameter.setValue(Float.parseFloat(new String(message.getPayload())));
        }
        else {
            //do nothing.
        }
    }
}
