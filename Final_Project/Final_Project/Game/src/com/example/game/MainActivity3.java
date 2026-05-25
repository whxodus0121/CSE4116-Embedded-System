package com.example.game;

import android.app.Activity;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;
import android.os.Handler;
import java.util.Random;
import android.content.Intent;

public class MainActivity3 extends Activity {

    private Button enemyButton;
    private Button meButton;
    private Button doubleDamageButton;
    private Button magnifierButton;
    private Button handcuffsButton;
    private Button recoveryButton;
    private TextView doubleDamageCount;
    private TextView magnifierCount;
    private TextView handcuffsCount;
    private TextView recoveryCount;

    // 아이템 개수 변수
    private int doubleDamageCountValue = 0;
    private int magnifierCountValue = 0;
    private int handcuffsCountValue = 0;
    private int recoveryCountValue = 0;

    // Double Damage on,off 여부
    private boolean isDoubleDamageActive = false;

    
    static {
        System.loadLibrary("native-lib");
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main_activity3);

        // UI 초기화
        enemyButton = (Button)findViewById(R.id.enemyButton);
        meButton = (Button)findViewById(R.id.meButton);
        doubleDamageButton = (Button)findViewById(R.id.doubleDamageButton);
        magnifierButton = (Button)findViewById(R.id.magnifierButton);
        handcuffsButton = (Button)findViewById(R.id.handcuffsButton);
        recoveryButton = (Button)findViewById(R.id.recoveryButton);
        doubleDamageCount = (TextView)findViewById(R.id.doubleDamageCount);
        magnifierCount = (TextView)findViewById(R.id.magnifierCount);
        handcuffsCount = (TextView)findViewById(R.id.handcuffsCount);
        recoveryCount = (TextView)findViewById(R.id.recoveryCount);

        // Double Damage 버튼 클릭 처리
        doubleDamageButton.setOnClickListener(new View.OnClickListener() {
            
        	private int clickCount = 0;
        	
        	@Override
        	public void onClick(View v) {
        		//아이템이 있다면 누를 때마다 on,off를 변경해서 유지해준다.
                clickCount++;
                if (doubleDamageCountValue>0&&clickCount % 2 == 1) {
                    isDoubleDamageActive = true;
                    Toast.makeText(MainActivity3.this, "Double Damage activated", Toast.LENGTH_SHORT).show();
                } else {
                    isDoubleDamageActive = false;
                    Toast.makeText(MainActivity3.this, "Double Damage deactivated", Toast.LENGTH_SHORT).show();
                }
                //on,off상태 보여주는 함수
                updateDoubleDamageUI();
                
            }
        });

        // Magnifier 버튼 클릭 처리
        magnifierButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {//아이템이 있다면 사용후 개수를 줄인다
                if (magnifierCountValue > 0) {
                    magnifierCountValue--;
                    magnifierCount.setText(String.valueOf(magnifierCountValue));
                    Toast.makeText(MainActivity3.this, "Magnifier used", Toast.LENGTH_SHORT).show();
                    //Magnigier사용했음을 전달하는 함수
                    sendCommandToNative("Magnifier",0);
                } else {
                    Toast.makeText(MainActivity3.this, "Not enough Magnifier items", Toast.LENGTH_SHORT).show();
                }
                
            }
        });

        // Handcuffs 버튼 클릭 처리
        handcuffsButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (handcuffsCountValue > 0) {//아이템이 있다면 사용후 개수를 줄인다
                    handcuffsCountValue--;
                    handcuffsCount.setText(String.valueOf(handcuffsCountValue));
                    Toast.makeText(MainActivity3.this, "Handcuffs used", Toast.LENGTH_SHORT).show();
                  //Handcuffs 사용했음을 전달하는 함수
                    sendCommandToNative("Handcuffs",0);
                } else {
                    Toast.makeText(MainActivity3.this, "Not enough Handcuffs items", Toast.LENGTH_SHORT).show();
                }
                
            }
        });

        // Recovery 버튼 클릭 처리
        recoveryButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (recoveryCountValue > 0) {//아이템이 있다면 사용후 개수를 줄인다
                    recoveryCountValue--;
                    recoveryCount.setText(String.valueOf(recoveryCountValue));
                    Toast.makeText(MainActivity3.this, "Recovery used", Toast.LENGTH_SHORT).show();
                  //Recovery 사용했음을 전달하는 함수
                    sendCommandToNative("Recovery",0);
                } else {
                    Toast.makeText(MainActivity3.this, "Not enough Recovery items", Toast.LENGTH_SHORT).show();
                }
              
            }
        });

        // Me 버튼 클릭 처리
        meButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
            	//Me버튼 눌렀을 때 shot_info를 구성해서 jni로 넘겨줌
                handleButtonClick("Me");
                //총을 발포하면 아이템의 개수를 랜덤으로 2개 추가
                increaseRandomItemCounts();
            }
        });

        // Enemy 버튼 클릭 처리
        enemyButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
            	//Enemy 버튼 눌렀을 때 shot_info를 구성해서 jni로 넘겨줌
                handleButtonClick("Enemy");
              //총을 발포하면 아이템의 개수를 랜덤으로 2개 추가
                increaseRandomItemCounts();
            }
        });
        
        startEndFlagChecker(); //게임이 끝나는 조건을 체크해준다.
    }

    private void handleButtonClick(String player) { //jni로 shot_info를 넘겨줌
    	
    	int damage = isDoubleDamageActive ? 2 : 1;
        sendCommandToNative(player, damage);
    	
    	if (isDoubleDamageActive) {// Double Damage가 활성화된 상태로 발포했다면 다시 비활성화 시키고 개수 감소시킴
            
            isDoubleDamageActive = false;
            updateDoubleDamageUI();
            Toast.makeText(MainActivity3.this, "Double Damage deactivated", Toast.LENGTH_SHORT).show();
       
            doubleDamageCountValue--;
            doubleDamageCount.setText(String.valueOf(doubleDamageCountValue));
        }
    	
        
    }

    // Double Damage, Recovery, Magnifier, Handcuffs 중 랜덤으로 2개 증가
    private void increaseRandomItemCounts() {
        Random random = new Random();
        int count = 0;

        while (count < 2) {
            int item = random.nextInt(4); // 0: Double Damage, 1: Recovery, 2: Magnifier, 3: Handcuffs
            switch (item) {
                case 0:
                    doubleDamageCountValue++;
                    doubleDamageCount.setText(String.valueOf(doubleDamageCountValue));
                    break;
                case 1:
                    recoveryCountValue++;
                    recoveryCount.setText(String.valueOf(recoveryCountValue));
                    break;
                case 2:
                    magnifierCountValue++;
                    magnifierCount.setText(String.valueOf(magnifierCountValue));
                    break;
                case 3:
                    handcuffsCountValue++;
                    handcuffsCount.setText(String.valueOf(handcuffsCountValue));
                    break;
            }
            count++;
        }
    }
    private void startEndFlagChecker() { //end_flag를 확인하여 게임을 끝내고 다시 대기화면(MainActivity)로 넘어가게 해주는 함수
        final Handler handler = new Handler();
        handler.postDelayed(new Runnable() {
            @Override
            public void run() {
                int endFlag = getEndFlag();
                if (endFlag == 1) {
                    Intent intent = new Intent(MainActivity3.this, MainActivity.class);
                    startActivity(intent);
                    finish(); // 현재 액티비티 종료
                } else {
                    handler.postDelayed(this, 1000); // 1초 후 다시 확인
                }
            }
        }, 1000); // 1초 후 확인 시작
    }
    
    
    
    private native void sendCommandToNative(String player, int damage);//드라이버에 명령을 전달하는 함수
    private native int getEndFlag(); //드라이버에서 end_flag받아오는 함수
    
    private void updateDoubleDamageUI() { //double Damage 아이템 사용여부를 표시해주는 함수
        if (isDoubleDamageActive) {
            doubleDamageButton.setText("Double Damage: ON");
        } else {
            doubleDamageButton.setText("Double Damage: OFF");
        }
    }
}
