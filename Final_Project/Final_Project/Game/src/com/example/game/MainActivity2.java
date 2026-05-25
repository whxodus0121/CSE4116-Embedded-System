package com.example.game;

import android.app.Activity;
import android.os.Bundle;
import android.view.View;
import android.widget.ArrayAdapter;
import android.widget.Spinner;
import android.widget.Toast;
import android.content.Intent;

public class MainActivity2 extends Activity {
	
    private Spinner hpSpinner;
    private static int selectedHp;
    
    static {
        System.loadLibrary("native-lib");
    }
    
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main_activity2);

        hpSpinner = (Spinner)findViewById(R.id.hp_spinner);

        // Spinner에 값을 설정
        ArrayAdapter<CharSequence> adapter = ArrayAdapter.createFromResource(this,
                R.array.hp_values, android.R.layout.simple_spinner_item);
        adapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
        hpSpinner.setAdapter(adapter);
    }

    // Done 버튼 클릭 시 호출될 메서드
    public void doneButtonClicked(View view) {
        selectedHp = Integer.parseInt(hpSpinner.getSelectedItem().toString()); // 선택된 HP 값을 정수로 저장
        Toast.makeText(this, "Selected Hp: " + selectedHp, Toast.LENGTH_SHORT).show();
        
        // 선택한 Hp를 드라이버에 전달하는 함수
        useSelectedHp(selectedHp);
        
        Intent intent = new Intent(this, MainActivity3.class);
        startActivity(intent);
    }

    //선택한 Hp를 드라이버에 전달하는 함수
    public native void useSelectedHp(int selectedHp);
    
    
}