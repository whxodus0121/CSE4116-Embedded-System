package com.example.game;


import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.view.View;

public class MainActivity extends Activity {
	
	static {
        System.loadLibrary("native-lib");
    }
	//LCD,FND,DOT의 기본 값들 출력해주는 함수 start누를 시 기본 값들 출력함
	public native void callSetCommand();
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
    }

    // 버튼 클릭 시 호출될 메서드
    public void startButtonClicked(View view) {
    	callSetCommand();
        Intent intent = new Intent(this, MainActivity2.class);
        startActivity(intent);
    }
}
