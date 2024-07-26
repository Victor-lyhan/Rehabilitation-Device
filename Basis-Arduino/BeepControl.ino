/*// 定义蜂鸣器引脚
const int buzzerPin = 12;

// 定义乐曲数组
int melody[] = {
  659, 659, 659, 523, 659, 783, 783, 659, 587,
  523, 659, 783, 783, 659, 587, 523, 659, 659,
  587, 523, 659, 587, 659, 0, 783, 880, 698,
  659, 523, 587, 659, 659, 587, 523, 659, 587,
  659, 0, 783, 880, 698, 659, 523, 587, 659,
  659, 587, 523, 659, 587, 659, 0, 783, 880,
  698, 659, 523, 587, 659
};

// 定义时值数组
int noteDurations[] = {
  4, 8, 8, 4, 4, 2, 4, 8, 8,
  4, 4, 2, 4, 8, 8, 4, 4, 4,
  4, 4, 4, 4, 2, 4, 8, 8, 4,
  4, 4, 4, 4, 2, 4, 8, 8, 4,
  4, 2, 4, 8, 8, 4, 4, 4, 4,
  4, 4, 4, 4, 2, 4, 8, 8, 4,
  4, 4, 4, 4
};

void setup() {
  // 设置蜂鸣器引脚为输出模式
  pinMode(buzzerPin, OUTPUT);
}

void loop() {
  // 逐音符播放乐曲
  for (int i = 0; i < sizeof(melody) / sizeof(melody[0]); i++) {
    // 计算音符持续时间
    int noteDuration = 1000 / noteDurations[i];

    // 播放音符
    tone(buzzerPin, melody[i], noteDuration);

    // 等待音符结束
    delay(noteDuration * 1.30);

    // 停止音符播放
    noTone(buzzerPin);
  }

   // 等待一段时间后重复播放
  delay(2000);
}*/

int buzzerPin = 12; // Connect the positive pin of the buzzer to digital pin 9

// Define the duration of each note in milliseconds
int duration = 300;

// Define the frequency of each pitch
int pitches[] = {
  261, // C
  293, // D
  329, // E
  349, // F
  392, // G
  440, // A
  493  // B
};

// Define the melody using an array of pitches and durations
char melody[] = "CGAGCGAGCGAGFGFED";

void setup() {
  pinMode(buzzerPin, OUTPUT);
}

void loop() {
  for (int i = 0; i < sizeof(melody); i++) {
    if (melody[i] != ' ') {
      int pitchIndex = melody[i] - 'A'; // Map letter to pitch index
      int frequency = pitches[pitchIndex];
      tone(buzzerPin, frequency, duration);
    } else {
      delay(duration); // Pause between notes
    }
    delay(duration / 2); // Add a small delay between each note
  }
}

