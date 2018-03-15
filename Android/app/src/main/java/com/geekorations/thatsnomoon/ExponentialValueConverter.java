package com.geekorations.thatsnomoon;

/**
 * Created by Cedric on 23/02/2018.
 */

public class ExponentialValueConverter implements IValueConverter {

    private double offset;
    private double multiplier;
    private double base;
    private double exponentFactor;

    public ExponentialValueConverter(double offset, double multiplier, double base, double exponentFactor){

        this.offset         = offset;
        this.multiplier     = multiplier;
        this.base           = base;
        this.exponentFactor = exponentFactor;

    }

    @Override
    public double convert(double in) {

        double result = offset + multiplier * Math.pow(base, in * exponentFactor);
        return result;
    }

    public double revert(double in) {

        double factor1 = (1.0/exponentFactor);

        double between = (in - offset)/multiplier;

        double factor2 = Math.log(between);
        double factor3 = 1.0 / Math.log(base);

        double result = factor1 * factor2 * factor3;

        return result;

    }
}
