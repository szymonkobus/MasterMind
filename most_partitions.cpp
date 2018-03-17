// MASTER MIND
// MAX PARTISION
// PREFER POSSIBLE

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
  /// do not alter these
  std::vector<int> sequence;
  int length;
  int num;
};

struct mm_solver{
  void init(int i_length, int i_num){                                           //DZIAŁA
    length = i_length;
    num = i_num;
                                                                                //zmienic poznej moze
    first = true;
    std::vector<int> temp;

    for(int place = 0; place < length; place++) {
      temp.push_back(0);
    }

    for(int i = 0; i < pow(num,length); i++){
      possible.push_back(temp);
      attempt_sequential(temp);
    }
  }

  /// do not alter the function interface (name, parameter list, void return)
  void create_attempt(std::vector<int>& attempt){
    std::vector<int> temp;

    if(first){
      if(length==4 && num>=2){
      temp.push_back(0);
      temp.push_back(0);
      temp.push_back(1);
      temp.push_back(1);
    }else{
      for(int i = 0; i < length; i++){
          temp.push_back(randn(num));
      }
    }
      attempt = temp;
      first = false;
    }else if(possible.size() == 1){
      attempt = possible[0];
    }else {
      std::vector<int> max_next_possible;
      std::vector<int> next_attempt;

      //initalize table_of_feedbacks
      std::vector<int> temp;
      std::vector< std::vector<int> > table_of_feedbacks(length+1);
      for(int row = 0; row <= length; row++){
        std::vector<int> temp1(length-row+1,0);
        table_of_feedbacks[row] = temp1;
        }

      /*
      for(int row = 0; row <= length; row++){
        table_of_feedbacks.push_back(temp);

        for(int col = 0; col <= length - row; col++){
          table_of_feedbacks[row].push_back(0);
        }
      }*/
      //initalize next_attempt
      for(int place = 0; place < length; place++){
        next_attempt.push_back(0);
      }

      int max;
      for(int i = 0; i < pow(num,length); i++){
        //0. Wyzeruj tabelke
        for(int row = 0; row <= length; row++){
          for(int col = 0; col <= length - row; col++){
            table_of_feedbacks[row][col] = 0;
          }
        }
        //1. Zewaluuj wszytkie mozliwe odpowiedzi wsadz do tabelki
        int black_hits;
        int white_hits;
        for(int i = 0; i < possible.size(); i++){
          give_feedback(next_attempt, possible[i], black_hits, white_hits);
          table_of_feedbacks[black_hits][white_hits]++;
        }
        //2. Znajdz maximum w tabelce
        max = 0;
        for(int row = 0; row <= length; row++){
          for(int col = 0; col <= length - row; col++){
            if(table_of_feedbacks[row][col] > 0){
              max++;
            }
          }
        }
        //3. Wsadz maximum do max_next_possible
        max_next_possible.push_back(max);
        attempt_sequential(next_attempt);
      }

      //1. Znajdz max w max_next_possible
      int index = 0;
      int globalMAX = max_next_possible[0];
      for(int i = 1; i < pow(num,length); i++){
        if(max_next_possible[i] > globalMAX){
          globalMAX = max_next_possible[i];
          index = i;
        }
      }
      bool possible_found = false;
      int temp_index;
      for(int i = 0; (i < possible.size()) && (!possible_found); i++) {
        temp_index = create_number_from_attempt(possible[i]);
        if(max_next_possible[temp_index] == globalMAX){
          index = temp_index;
          possible_found = true;
        }
      }
      //2. Na podstawie i stworz attempt
      create_attempt_from_number(attempt,index);
    }
  }

  /// this member function acquires the feedback about a certain attempt
  /// (see the other examples provided for clarifications)
  /// do not alter the function interface (name, parameter list, void return)
  void learn(std::vector<int>& attempt, int black_hits, int white_hits){        //DZIAŁA
    std::vector< std::vector<int> > temp;
    int b_hits;
    int w_hits;
    for(int i = 0; i < possible.size(); i++){
      give_feedback(attempt,possible[i], b_hits, w_hits);
      if((b_hits == black_hits) && (w_hits == white_hits)){
        temp.push_back(possible[i]);
      }
    }
    possible = temp;
  }

  int length;
  int num;
  std::vector< std::vector<int> > possible;
  bool first;
  /// you may add other member functions and member data as needed
  /// (keep in mind the distinction between member function variables
  /// and member data of the struct)
  void attempt_sequential(std::vector<int>& next_attempt){                      //DZIAŁA
    bool carry = true;
    for(int place = length - 1; (place >= 0) && carry; place--){
      carry = false;
      next_attempt[place]++;
      if(next_attempt[place] == num){
        next_attempt[place] = 0;
        carry = true;
      }
    }
  }

  void give_feedback(const std::vector<int>& attempt, const std::vector<int>& prev_possible,
                   int& black_hits, int& white_hits){
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
      numFreqSequence[prev_possible[j]]++;
      if(attempt[j] == prev_possible[j]){
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

  void create_attempt_from_number(std::vector<int>& attempt, int index){        //DZIAŁA
    std::vector<int> temp(length);
    for(int place = length - 1; place >= 0; place--){
      temp[place] = index % num;
      index = index / num;
    }
    attempt = temp;
  }

  int create_number_from_attempt(std::vector<int> attempt){
    int sum = attempt[length-1];
    for(int place = length - 2; place >= 0; place--){
      sum *= num;
      sum += attempt[place];
    }
    return sum;
  }
};

/// before the submission you need to remove the main
/// (either delete it or comment it out)
/// otherwise it will intefere with the automated testing

int main(){
  set_random_seed();
  int length, num;
  std::cout << "enter length of sequence and number of possible values:" << std::endl;
  std::cin >> length >> num;
  std::vector<int> attemptsN;
  std::vector<float> times;
  for (int i = 0; i < pow(num,length); i++) {
    mm_solver solver;
    solver.init(length, num);
    mm_code_maker maker;
    maker.init(length, num);
    maker.generate_sequence(i);

    int black_hits = 0, white_hits=0;
    int attempts_limit = 10;
    int attempts = 0;
    clock_t start = clock();

    while((black_hits < length) && (attempts < attempts_limit)){
        std::vector<int> attempt;
        solver.create_attempt(attempt);
        maker.give_feedback(attempt, black_hits, white_hits);
        /*
        std::cout << "attempt: " << std::endl;
        for(int i = 0; i < attempt.size(); i++){
            std::cout << attempt[i] << " ";
        }
        std::cout << std::endl;
        std::cout << "black pegs: " << black_hits << " " << " white pegs: " << white_hits << std::endl;
        */
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

    attemptsN.push_back(attempts);
    times.push_back(seconds);
  }
  int sumN = 0;
  int maxN = attemptsN[0];
  float sumT = 0.0;
  for(int i = 0; i < attemptsN.size(); i++){
    sumN += attemptsN[i];
    if(attemptsN[i]>maxN) maxN = attemptsN[i];
    sumT += times[i];
  }
  float averageN = float(sumN) / attemptsN.size();
  float averageT = sumT / attemptsN.size();
  std::cout << "Average guesses: " << averageN << '\n';
  std::cout << "Maximal guesses: " << maxN << '\n';
  std::cout << "Average time: " << averageT << '\n' << '\n';

  std::vector<int> guessFreq(maxN+1);

  for(int i = 0; i < maxN; i++) guessFreq[i] = 0;
  for(int i = 0; i < attemptsN.size(); i++) guessFreq[attemptsN[i]]++;

  std::cout << "Guess frequency" << '\n';
  for(int i = 1; i < guessFreq.size(); i++) {
    std::cout << i << ". " << guessFreq[i] << '\n';
  }
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
