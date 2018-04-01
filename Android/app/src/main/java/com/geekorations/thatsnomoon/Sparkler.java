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

public class Sparkler extends LedCard {

    public Parameter brightnessParameter;
    public Parameter variationParameter;
    public Parameter smoothingParameter;

    private int maxBrightness;
    private int maxVariation;
    private int maxSmoothing;

    private int startBrightness;
    private int startVariation;
    private int startSmoothing;


    public  Sparkler(Context context) {
        super(context);

        initializeViews(context);
    }

    public Sparkler(Context context, AttributeSet attrs) {
        super(context, attrs);

        TypedArray a = context.obtainStyledAttributes(attrs, R.styleable.Sparkler);

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
        inflater.inflate(R.layout.ledcard_view, this);
    }

    protected void onFinishInflate() {
        super.onFinishInflate();

        this.brightnessParameter     = new Parameter(this.getContext());
        this.variationParameter      = new Parameter(this.getContext());
        this.smoothingParameter      = new Parameter(this.getContext());

        this.brightnessParameter.setBarMax(maxBrightness);
        this.variationParameter.setBarMax(maxVariation);
        this.smoothingParameter.setBarMax(maxSmoothing);

        this.brightnessParameter.setName("Brightness");
        this.variationParameter.setName("Variation");
        this.smoothingParameter.setName("Smoothing");

        this.smoothingParameter.setValueConverter(new ExponentialValueConverter(1,-1,2,-1.0/125.0));

        this.brightnessParameter.setValue(678);
        this.variationParameter.setValue(123);
        this.smoothingParameter.setValue(0.899);

        Parameter[] parameters = new Parameter[] {
                brightnessParameter,
                variationParameter,
                smoothingParameter
        };

        this.setParameters(parameters);
    }


    public void setMQTTPublishTopic(String topic) {

        this.mqttPublishTopic = topic;
        this.brightnessParameter.setMQTTTopic(this.mqttPublishTopic + "/" +"brightness");
        this.variationParameter.setMQTTTopic(this.mqttPublishTopic + "/" +"variation");
        this.smoothingParameter.setMQTTTopic(this.mqttPublishTopic + "/" +"smoothing");

    }

    public void processMessage(String topic, MqttMessage message) {

        if (topic.equals(this.mqttReceiveTopic + "/" +"brightness")) {
            this.brightnessParameter.setValue(Float.parseFloat(new String(message.getPayload())));
        }
        else if (topic.equals(this.mqttReceiveTopic + "/" + "variation")) {
            this.variationParameter.setValue(Float.parseFloat(new String(message.getPayload())));
        }
        else if (topic.equals(this.mqttReceiveTopic + "/" + "smoothing")) {
            this.smoothingParameter.setValue(Float.parseFloat(new String(message.getPayload())));
        }
        else {
            //do nothing.
        }
    }
}
