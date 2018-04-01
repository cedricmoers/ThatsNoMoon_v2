package com.geekorations.thatsnomoon;

import android.support.v4.view.PagerAdapter;
import android.view.View;
import android.view.ViewGroup;

/**
 * Created by Cedric on 1/04/2018.
 */

public class CardPagerAdapter extends PagerAdapter {

    public Object instantiateItem(ViewGroup collection, int position) {

        int resId = 0;
        switch (position) {
            case 0:
                resId = R.id.thrustersSparkler;
                break;
            case 1:
                resId = R.id.stars1Sparkler;
                break;
            case 2:
                resId = R.id.stars2Sparkler;
                break;
            case 3:
                resId = R.id.stars3Sparkler;
                break;
            case 4:
                resId = R.id.incomingShipBlinker;
                break;
            case 5:
                resId = R.id.landingShipBlinker;
                break;
            case 6:
                resId = R.id.landingPlatformSpots;
                break;
        }
        return  collection.findViewById(resId);
    }

    @Override
    public int getCount() {
        return 7;
    }

    @Override
    public boolean isViewFromObject(View arg0, Object arg1) {
        return arg0 == arg1;
    }

    @Override
    public void destroyItem(ViewGroup collection, int position, Object view) {
        collection.removeView((View) view);
    }
}
