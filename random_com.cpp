#include <iostream>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <cmath>


void set_random_seed();
int randn(int n);

struct mm_code_maker{
    void init(int i_length, int i_num){
        length = i_length;
        num = i_num;
    }

    void generate_sequence(){
        for(int i = 0; i < length; i++){
            sequence.push_back(randn(num));
        }
    }
    //moje dodane
    void generate_sequence(int i){
      std::vector<int> temp(length);
      for(int place = length - 1; place >= 0; place--){
        temp[place] = i % num;
        i = i / num;
      }
      sequence = temp;
    }

    void give_feedback(const std::vector<int>& attempt, int& black_hits, int& white_hits){
        black_hits = 0;
        white_hits = 0;
        int* numFreqAttempt = new int[num];
        int* numFreqSequence = new int[num];

        for(int i = 0; i < num; i++){
          numFreqAttempt[i] = 0;
          numFreqSequence[i] = 0;
        }

        for(int j = 0; j < length; j++){
          numFreqAttempt[attempt[j]]++;
          numFreqSequence[sequence[j]]++;
          if(attempt[j] == sequence[j]){
            black_hits++;
          }
        }
        int hits = 0;
        for(int k = 0; k < num; k++) {
          if(numFreqAttempt[k]>numFreqSequence[k]){
            hits += numFreqSequence[k];
          }else{
            hits += numFreqAttempt[k];
          }
        }
        white_hits = hits - black_hits;
        delete numFreqAttempt;
        delete numFreqSequence;
    }

    std::vector<int> sequence;
    int length;
    int num;
};

struct mm_solver{
    void init(int i_length, int i_num){
        length = i_length;
        num = i_num;
        guesses = 0;
    }

    void create_attempt(std::vector<int>& attempt){
      bool ready = false;
      while(!ready){
          for(int i = 0; i < length; i++){
              attempt.push_back(randn(num));
          }
          guesses++;
          bool notpossible = false;
          for(int i = past_attempts.size()-1 ; i >= 0 && !notpossible; i--){
              if(attempt == past_attempts[i]){
                  notpossible = true;
              }
              if(!feedback_possible(attempt, past_attempts[i], black[i], white[i])){
                notpossible = true;
              }
          }
          if(notpossible){
              attempt.clear();
          }
          else{
              ready = true;
          }
      }
      //std::cout << "Guess nr: " << guesses << '\n';
    }

    void learn(std::vector<int>& attempt, int black_hits, int white_hits){
      if(black_hits != attempt.size()){
          past_attempts.push_back(attempt);
          black.push_back(black_hits);
          white.push_back(white_hits);
      }
    }

    bool feedback_possible(const std::vector<int>& attempt, const std::vector<int>& past_att, int black_hits_rec, int white_hits_rec){
        int black_hits = 0;
        int white_hits = 0;
        char* bw = new char[length];
        for(int i = 0; i < length; i++){          //mark black hits
          bw[i] = 'n';
          if(attempt[i] == past_att[i]){
            bw[i] = 'b';
          }
        }
        for(int att = 0; att < length; att++){        //mark white hits
          if(bw[att] != 'b'){
            bool change = false;
            for(int seq = 0; (seq < length) && (!change); seq++) {
              if((past_att[seq] == attempt[att]) && (bw[seq] == 'n')){
                bw[seq] = 'w';
                change = true;
              }
            }
          }
        }
        for (int i = 0; i < length; i++) {
          if(bw[i] == 'b') black_hits++;
          else if(bw[i] == 'w') white_hits++;
        }
        return ((black_hits == black_hits_rec) && (white_hits == white_hits_rec));
        delete bw;
    }

    int length;
    int num;
    std::vector<std::vector<int> > past_attempts;
    std::vector<int> black;
    std::vector<int> white;

    long int guesses;
};

/// before the submission you need to remove the main
/// (either delete it or comment it out)
/// otherwise it will intefere with the automated testing



int main(){
    set_random_seed();
    int length, num;
    std::cout << "enter length of sequence and number of possible values:" << std::endl;
    std::cin >> length >> num;

    mm_solver solver;
    solver.init(length, num);

    mm_code_maker maker;
    maker.init(length, num);
    maker.generate_sequence();

    int black_hits=0, white_hits=0;
    int attempts_limit = 5000;
    int attempts = 0;
    clock_t start = clock();
    while((black_hits < length) && (attempts < attempts_limit)){
        std::vector<int> attempt;
        solver.create_attempt(attempt);
        maker.give_feedback(attempt, black_hits, white_hits);
        std::cout << "attempt: " << std::endl;
        for(int i = 0; i < attempt.size(); i++){
            std::cout << attempt[i] << " ";
        }
        std::cout << std::endl;
        std::cout << "black pegs: " << black_hits << " " << " white pegs: " << white_hits << std::endl;
        solver.learn(attempt, black_hits, white_hits);
        attempts++;
    }
    clock_t end = clock();
    float seconds = (float)(end - start) / CLOCKS_PER_SEC;
    std::cout << '\n';
    std::cout << "time: " << seconds << '\n';
    if(black_hits == length){
        std::cout << "the solver has found the sequence in " << attempts << " attempts" << std::endl;
    }
    else{
        std::cout << "after " << attempts << " attempts still no solution" << std::endl;
    }
    std::cout << "the sequence generated by the code maker was:" << std::endl;
    for(int i = 0; i < maker.sequence.size(); i++){
        std::cout << maker.sequence[i] << " ";
    }
    std::cout << std::endl;
    return 0;
}



/// not a great implementation for set_random_seed and for randn;
/// if you are trying to get better results you may want to change
/// the implementation using C++11 features, see for instance
/// https://isocpp.org/files/papers/n3551.pdf
/// but don't change the interface/declaration of the functions

void set_random_seed(){
    std::srand(std::time(0));
}

int randn(int n){
    return std::rand() % n;
}

/// add here the implementation for any other functions you wish to define and use
