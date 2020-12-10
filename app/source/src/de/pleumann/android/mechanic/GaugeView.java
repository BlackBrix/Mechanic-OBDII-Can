/*********************************************************************
 * Mechanic - Hacking your car
 *
 * Copyright (C) 2013 Joerg Pleumann
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * LICENSE file for more details.
 */package de.pleumann.android.mechanic;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Paint;
import android.graphics.Paint.Align;
import android.graphics.Paint.Style;
import android.graphics.RectF;
import android.util.AttributeSet;
import android.view.View;

public class GaugeView extends View {

    private float min = -40;
    private float max = 215;
    private float value = 0;
    private float target = 88;
    private String unit = "¡C";

    private Paint paint = new Paint();
    private RectF rect = new RectF();

    public GaugeView(Context context) {
        super(context);
    }

    public GaugeView(Context context, AttributeSet attrs) {
        super(context, attrs);
    }

    public GaugeView(Context context, AttributeSet attrs, int defStyle) {
        super(context, attrs, defStyle);
    }

    public void setMin(float value) {
        this.min = value;
        postInvalidate();
    }

    public void setMax(float value) {
        this.max = value;
        postInvalidate();
    }
    
    public void setValue(float value) {
        this.value = value;
        postInvalidate();
    }

    public void setTarget(float target) {
        if (target > max) {
            target = max;
        } else if (target < min) {
            target = min;
        }
        
        this.target = target;
        postInvalidate();
    }
    
    public void setUnit(String unit) {
        this.unit = unit;
        postInvalidate();
    }
    
    public void onStep() {
        float delta = target - value;
        if (delta >= 0.01f || delta <= -0.01f) {
            value = value + delta / 10.0f;
            postInvalidate();
        }
    }
    
    @Override
    public void onDraw(Canvas canvas) {
        super.onDraw(canvas);

        float radius = Math.min(getWidth(), getHeight()) / 2;

        paint.reset();
        paint.setStyle(Style.STROKE);
        paint.setAntiAlias(true);
        paint.setColor(0xFFFFFFFF);
        paint.setTextAlign(Align.CENTER);
        paint.setTextSize(radius / 5);

        canvas.translate(radius, radius);
        
        radius -= 10;

        canvas.drawText("" + (int)value + unit, 0, radius * 4 / 5, paint);

        paint.setStrokeWidth(10);

        rect.set(-radius, -radius, radius, radius);
        
        canvas.drawArc(rect, -225f, 270f, false,
                paint);

        canvas.rotate(-292.5f, 0, 0);

        for (int i = 0; i < 5; i++) {
            canvas.rotate(67.5f, 0, 0);
            canvas.drawLine(radius - 20, 0, radius + 4.5f, 0, paint);
        }
        
        canvas.rotate(-270, 0, 0);
        paint.setColor(0xffff4040);
        
        canvas.rotate(270 * (value - min) / (max - min));
        canvas.drawLine(- radius / 3, 0, radius - 40, 0, paint);

        paint.setColor(0xffFFFFFF);
        paint.setStyle(Style.FILL_AND_STROKE);
        rect.set(-5, -5, 5, 5);
        canvas.drawArc(rect, 0f, 360f, true,
                paint);
        
    }

}
