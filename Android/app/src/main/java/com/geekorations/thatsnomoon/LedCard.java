package com.geekorations.thatsnomoon;

import android.content.Context;
import android.content.res.TypedArray;
import android.graphics.Typeface;
import android.util.AttributeSet;
import android.view.LayoutInflater;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.TextView;

import org.eclipse.paho.android.service.MqttAndroidClient;
import org.eclipse.paho.client.mqttv3.MqttMessage;

import java.sql.ParameterMetaData;

/**
 * Created by Cedric on 31/03/2018.
 */

public class LedCard extends LinearLayout {

    protected String name;
    protected int pictureResource;
    private TextView nameTextView;
    private TextView foreignNameTextView;
    private LinearLayout parameterContainer;
    private ImageView imageView;
    protected String mqttReceiveTopic;
    protected String mqttPublishTopic;
    protected MqttAndroidClient mqttClient;
    protected Parameter[] parameters;

    public  LedCard(Context context) {
        super(context);

        this.name               = "";
        this.pictureResource    = R.drawable.thatsnomoon;

        initializeViews(context);
    }

    public LedCard(Context context, AttributeSet attrs) {
        super(context, attrs);

        TypedArray attributes   =  context.obtainStyledAttributes(attrs, R.styleable.LedCard);

        this.name               =  attributes.getString(R.styleable.LedCard_name);
        this.pictureResource    =  attributes.getResourceId(R.styleable.LedCard_pictureSource, R.drawable.thatsnomoon);

        attributes.recycle();

        initializeViews(context);
    }

    private void initializeViews(Context context) {
        LayoutInflater inflater = (LayoutInflater) context
                .getSystemService(Context.LAYOUT_INFLATER_SERVICE);
        inflater.inflate(R.layout.ledcard_view, this);
    }

    protected void onFinishInflate() {
        super.onFinishInflate();

        //Set references to the relevant views.
        this.nameTextView               = (TextView) this.findViewById(R.id.nameTextView);
        this.foreignNameTextView        = (TextView) this.findViewById(R.id.foreignNameTextView);
        this.parameterContainer         = (LinearLayout)findViewById(R.id.parameterListViewAsLinearLayout);
        this.imageView                  = (ImageView) findViewById(R.id.imageView);

        Typeface font = Typeface.createFromAsset(getContext().getAssets(), "fonts/Aurebesh.ttf");
        this.foreignNameTextView.setTypeface(font, Typeface.NORMAL);

        //update the view with the given values.
        setName(this.name);
        setImageResource(this.pictureResource);
    }

    public void setImageResource(int imageResource) {
        if (imageResource != 0) {
            this.imageView.setImageResource(imageResource);
        }
        else {
            throw new NullPointerException("Cannot set the image resource. The resource does not exist.");
        }
    }

    public void setName(String name) {
        //Check if the input name is valid.
        if (name != null && !name.trim().isEmpty())
        {
            //Set the name of this object to the input.
            this.name = name;
        }
        else {
            //Set the name of this object to "Unnamed Parameter"
            this.name = "Unnamed Parameter";
        }

        //Update the textviews
        this.nameTextView.setText(this.name);
        this.foreignNameTextView.setText(this.name);

    }

    public void setParameters(Parameter[] parameters) {

        //set the parameters within the linear layout
        if (parameters != null) {

            this.parameters = parameters;

            if (this.parameterContainer != null) {

                //Remove all the children of the parameter container.
                this.parameterContainer.removeAllViews();

                //Add each parameter to the parameter container.
                for (Parameter parameter : parameters) {

                    parameterContainer.addView(parameter);

                }
            }
            else {
                throw new NullPointerException("Cannot set parameters: The parameter container is not referenced.");
            }
        }
        else {
            throw new NullPointerException("Cannot set parameters: The parameters input is not referenced.");
        }
    }

    public void setMQTTClient(MqttAndroidClient client) {
        if (client != null) {
            this.mqttClient = client;

            for (Parameter parameter : this.parameters) {
                parameter.setMQTTClient(client);
            }
        }
        else {
            throw new NullPointerException("Cannot set MQTT client: The given client is not referenced.");
        }
    }

    public void setMQTTReceiveTopic(String topic) {
        //Check if the input name is valid.
        if (name != null && !name.trim().isEmpty())
        {
            //Set the name of this object to the input.
            this.mqttReceiveTopic = topic;
        }
        else {
            throw new NullPointerException("Cannot set MQTT receive topic: The topic cannot be empty.");
        }
    }

    public void processMessage(String topic, MqttMessage message) {

            //do nothing.
    }
}
