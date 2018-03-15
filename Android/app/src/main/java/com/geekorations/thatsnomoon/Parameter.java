package com.geekorations.thatsnomoon;

import android.content.Context;
import android.content.res.TypedArray;
import android.util.AttributeSet;
import android.view.LayoutInflater;
import android.widget.LinearLayout;
import android.widget.SeekBar;
import android.widget.TextView;

import org.eclipse.paho.android.service.MqttAndroidClient;
import org.eclipse.paho.client.mqttv3.MqttException;

import java.util.Timer;
import java.util.TimerTask;

/**
 * Created by Cedric on 4/02/2018.
 */

public class Parameter extends LinearLayout {

    private SeekBar valueSeekBar;
    private TextView nameTextView;
    private TextView actualValueTextView;
    private String MQTTTopic = "";
    private MqttAndroidClient MQTTClient;

    Timer timer;

    String parameterName;
    int maxValue;
    int startValue;
    int scaling;

    IValueConverter valueConverter;

    public  Parameter(Context context) {
        super(context);
        initializeViews(context);
    }

    public Parameter(Context context, AttributeSet attrs) {
        super(context, attrs);


            TypedArray a = context.obtainStyledAttributes(attrs, R.styleable.Parameter);

            parameterName = a.getString(R.styleable.Parameter_parameterName);
            maxValue = a.getInteger(R.styleable.Parameter_maxValue, 1000);
            startValue = a.getInteger(R.styleable.Parameter_startValue, 500);
            scaling = a.getInteger(R.styleable.Parameter_scaling, 1);

            a.recycle();

        initializeViews(context);


    }

    private void initializeViews(Context context) {
        LayoutInflater inflater = (LayoutInflater) context
                .getSystemService(Context.LAYOUT_INFLATER_SERVICE);
        inflater.inflate(R.layout.parameter_view, this);
    }


    protected void onFinishInflate() {
        super.onFinishInflate();

        this.valueConverter = null;

        valueSeekBar = (SeekBar) this.findViewById(R.id.valueSeekBar);
        nameTextView = (TextView) this.findViewById(R.id.nameTextView);
        actualValueTextView = (TextView) this.findViewById(R.id.actualValueTextView);

        nameTextView.setText(parameterName);
        valueSeekBar.setMax(maxValue);
        valueSeekBar.setProgress(startValue);

    }

    public void setName(String name) {
        this.nameTextView.setText(name);
    }

    public void setBarValue(int value) {

        this.valueSeekBar.setProgress(value);
    }

    public void setBarMax(int value) {

        this.valueSeekBar.setMax(value);
    }

    public void setValue(double value) {

        double revertedValue;

        if (valueConverter != null) {
            revertedValue = this.valueConverter.revert(value);
        }
        else {
            revertedValue =value;
        }

        setBarValue((int)revertedValue);

        setActualValue(formatNumericString(value));

    }

    public void setActualValue(int value) {

        this.actualValueTextView.setText(Integer.toString(value));
    }

    public void setActualValue(double value) {

        this.actualValueTextView.setText(Double.toString(value));
    }

    public void setActualValue(String value) {

        this.actualValueTextView.setText(value);
    }

    public void setMQTTTopic(String topic) {

        this.MQTTTopic = topic;

    }

    public void setValueConverter(IValueConverter valueConverter) {

        this.valueConverter = valueConverter;

    }

    public void setMQTTClient(MqttAndroidClient client) {

        MQTTClient = client;

        valueSeekBar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {

                timer.cancel(); //Stop the scheduled tasks of the timer.

                //When releasing, publish the current value;

                double value;

                if (valueConverter != null) {
                    value = valueConverter.convert((double) valueSeekBar.getProgress());
                }
                else {
                    value = (double) valueSeekBar.getProgress();
                }

                String textualValue = formatNumericString(value);

                publish(MQTTTopic, textualValue);
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {

                //When holding the timer, publish an update of its value every x ms.
                timer = new Timer();// timer needs to be recreated after cancel.

                timer.scheduleAtFixedRate(new TimerTask() {
                    @Override
                    public void run() {

                        double value;

                        if (valueConverter != null) {
                            value = valueConverter.convert((double) valueSeekBar.getProgress());
                        }
                        else {
                            value = (double) valueSeekBar.getProgress();
                        }
                        String textualValue = formatNumericString(value);

                        publish(MQTTTopic, textualValue);

                    }
                }, 0, 500);
            }

            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {

            }
        });
    }

    public void publish(String topic, String message){

        try
        {
            MQTTClient.publish(topic, message.getBytes(), 0, false);
        }
        catch (MqttException e)
        {
            e.printStackTrace();
        }

    }

    private String formatNumericString(double d)
    {
        if(d == (long) d)
            return String.format("%d",(long)d);
        else
            return String.format("%.7s",d);
    }


}
