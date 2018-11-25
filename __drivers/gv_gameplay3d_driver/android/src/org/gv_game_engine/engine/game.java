/*
 * Copyright (C) 2009 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
package org.gv_game_engine.engine;

import android.app.Activity;
import android.app.NativeActivity;
import android.widget.TextView;

import android.content.Intent;
import android.content.IntentSender.SendIntentException;
import android.content.ServiceConnection;

import android.os.Bundle;
import android.os.Handler;
import android.os.IBinder;
import android.os.RemoteException;

import android.text.TextUtils;
import android.util.Log;

import java.util.ArrayList;
import java.util.List;


public class game extends NativeActivity
{
    /** Called when the activity is first created. */
	public static game instance ;

    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
		instance= this;
        /* Create a TextView and set its content.
         * the text is retrieved by calling a native
         * function.
         */
        /*TextView  tv = new TextView(this);
        tv.setText( "hello  );
        setContentView(tv);*/
    }

	public  void LaunchURL(String URL)
	{
		try
		{
			Intent BrowserIntent = new Intent(Intent.ACTION_VIEW, android.net.Uri.parse(URL));
			startActivity(BrowserIntent);
		}
		catch (Exception e)
		{
			///log.("LaunchURL failed with exception " + e.getMessage());
		}
	}


	public static  game StaticJavaGameInstance()
	{
		return instance;
	}



    /* A native method that is implemented by the
     * 'hello-jni' native library, which is packaged
     * with this application.
     */
    //public native String  stringFromJNI();

    /* This is another native method declaration that is *not*
     * implemented by 'hello-jni'. This is simply to show that
     * you can declare as many native methods in your Java code
     * as you want, their implementation is searched in the
     * currently loaded native libraries only the first time
     * you call them.
     *
     * Trying to call this function will result in a
     * java.lang.UnsatisfiedLinkError exception !
     */
    //public native String  unimplementedStringFromJNI();

    /* this is used to load the 'hello-jni' library on application
     * startup. The library has already been unpacked into
     * /data/data/com.example.hellojni/lib/libhello-jni.so at
     * installation time by the package manager.
     */
    //static {
        //System.loadLibrary("hello-jni");
    //}
}
