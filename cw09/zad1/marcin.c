#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

#define EMPTY 0
#define BOARDED 1

pthread_mutex_t mutex_cart_dock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t carriage_order = PTHREAD_COND_INITIALIZER;

pthread_mutex_t mutex_passenger = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t current_passanger_change = PTHREAD_COND_INITIALIZER;

pthread_mutex_t wait_for_release = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t leaver = PTHREAD_COND_INITIALIZER;

pthread_mutex_t end = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t end_cond = PTHREAD_COND_INITIALIZER;

pthread_mutex_t button = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t button_cond = PTHREAD_COND_INITIALIZER;

int passengers_count;
int carriages_count;
int routes;
int carriage_capacity;

int correct_cariage_id;
int all_loaded = 0;

int current_passanger = -2;

int current_leaver = -1;

int button_presser = -1;
int wait_for_button;

int active_carriages;

int* carriage_status;

int* passengers_queue;

int current_capacity;

void printf_time_stamp() {
  struct timeval time_stamp;
  gettimeofday(&time_stamp, NULL);
  struct tm* ptm;
  ptm = localtime(&time_stamp.tv_sec);

  char time_string[40];

  strftime(time_string, sizeof(time_string), "%H:%M:%S", ptm);
  printf("%s.%06ld", time_string, time_stamp.tv_usec);
}

void add_to_queue(int passenger) {
  for (int i = 0; i < passengers_count; ++i) {
    if (passengers_queue[i] == -1) {
      passengers_queue[i] = passenger;
      break;
    }
  }
}

int get_from_queue() {
  int result = passengers_queue[0];
  for (int i = 1; i < passengers_count; ++i) {
    passengers_queue[i - 1] = passengers_queue[i];
  }

  passengers_queue[passengers_count - 1] = -1;

  return result;
}

void* carriage_routine(void* arg) {
  int id = *((int*)arg);

  int* passengers = calloc(carriage_capacity, sizeof(int));

  for (int i = 0; i < routes; ++i) {
    pthread_mutex_lock(&mutex_cart_dock);

    while (correct_cariage_id != id && all_loaded != 0) {
      pthread_cond_wait(&carriage_order, &mutex_cart_dock);
    }

    button_presser = -1;

    printf_time_stamp();
    printf(" carriage %d wait for passengers.\n", id);

    pthread_mutex_lock(&mutex_passenger);
    for (int j = 0; j < carriage_capacity; ++j) {
      current_capacity = j;
      current_passanger = get_from_queue();

      passengers[j] = current_passanger;
      pthread_cond_broadcast(&current_passanger_change);
      while (current_passanger != -1) {
        pthread_cond_wait(&current_passanger_change, &mutex_passenger);
      }
    }
    pthread_mutex_unlock(&mutex_passenger);

    printf_time_stamp();
    printf(" carriage %d closed door.\n", id);

    pthread_mutex_lock(&button);

    button_presser = passengers[rand() % carriage_capacity];

    pthread_cond_broadcast(&button_cond);

    wait_for_button = 1;

    while (wait_for_button == 1) {
      pthread_cond_wait(&button_cond, &button);
    }

    pthread_mutex_unlock(&button);

    carriage_status[id] = BOARDED;

    correct_cariage_id = (correct_cariage_id + 1) % carriages_count;

    int cnt = 0;
    for (int i = 0; i < carriages_count; ++i) {
      if (carriage_status[i] == BOARDED) {
        cnt++;
      }
    }

    if (cnt == carriages_count) {
      all_loaded = 1;
    }

    pthread_cond_broadcast(&carriage_order);
    pthread_mutex_unlock(&mutex_cart_dock);

    printf_time_stamp();
    printf(" carriage %d started route.\n", id);

    usleep(150);

    pthread_mutex_lock(&mutex_cart_dock);

    while (correct_cariage_id != id && all_loaded == 0) {
      pthread_cond_wait(&carriage_order, &mutex_cart_dock);
    }

    printf_time_stamp();
    printf(" carriage %d ended route and opened door.\n", id);

    for (int j = 0; j < carriage_capacity; ++j) {
      pthread_mutex_lock(&wait_for_release);
      current_leaver = passengers[j];
      add_to_queue(passengers[j]);
      passengers[j] = -1;
      current_capacity = carriage_capacity - j - 1;
      pthread_cond_broadcast(&leaver);
      while (current_leaver != -1) {
        pthread_cond_wait(&leaver, &wait_for_release);
      }

      pthread_mutex_unlock(&wait_for_release);
    }

    carriage_status[id] = EMPTY;

    correct_cariage_id = (correct_cariage_id + 1) % carriages_count;

    cnt = 0;
    for (int i = 0; i < carriages_count; ++i) {
      if (carriage_status[i] == EMPTY) {
        cnt++;
      }
    }

    if (cnt == carriages_count) {
      all_loaded = 1;
    }
    pthread_cond_broadcast(&carriage_order);
    pthread_mutex_unlock(&mutex_cart_dock);
  }

  pthread_mutex_lock(&mutex_passenger);

  active_carriages -= 1;

  pthread_cond_broadcast(&current_passanger_change);

  pthread_mutex_unlock(&mutex_passenger);

  free(passengers);

  return NULL;
}

void* passenger_routine(void* arg) {
  int id = *((int*)arg);

  active_carriages = carriages_count;

  while (1) {
    pthread_mutex_lock(&mutex_passenger);

    while (current_passanger != id) {
      if (active_carriages == 0) {
        pthread_mutex_unlock(&mutex_passenger);
        return NULL;
      }

      pthread_cond_wait(&current_passanger_change, &mutex_passenger);
    }

    printf_time_stamp();
    printf(" passendger %d boarder carriage. Current passengers %d.\n", id,
           current_capacity);

    current_passanger = -1;
    pthread_cond_broadcast(&current_passanger_change);
    pthread_mutex_unlock(&mutex_passenger);

    pthread_mutex_lock(&button);

    while (button_presser == -1) {
      pthread_cond_wait(&button_cond, &button);
    }

    if (button_presser == id) {
      printf_time_stamp();
      printf(" passendger %d pressed button.\n", id);

      wait_for_button = 0;
      pthread_cond_broadcast(&button_cond);
    }

    pthread_mutex_unlock(&button);

    pthread_mutex_lock(&wait_for_release);

    while (current_leaver != id) {
      pthread_cond_wait(&leaver, &wait_for_release);
    }

    printf_time_stamp();
    printf(" passendger %d left carriage. Current passengers %d.\n", id,
           current_capacity);

    current_leaver = -1;
    pthread_cond_broadcast(&leaver);
    pthread_mutex_unlock(&wait_for_release);
  }

  return NULL;
}

int main(int argc, char** argv) {
  if (argc != 5) {
    printf("Too few arguments. Usage /main p w c n. \n");
    exit(1);
  }

  if (sscanf(argv[1], "%d", &passengers_count) != 1) {
    printf("Cant parse passenger_count. \n");
    exit(2);
  }

  if (sscanf(argv[2], "%d", &carriages_count) != 1) {
    printf("Cant parse carriages_count. \n");
    exit(2);
  }

  if (sscanf(argv[3], "%d", &carriage_capacity) != 1) {
    printf("Cant parse carriage_capacity. \n");
    exit(2);
  }

  if (sscanf(argv[4], "%d", &routes) != 1) {
    printf("Cant parse routes. \n");
    exit(2);
  }

  if (passengers_count < carriage_capacity * carriages_count) {
    printf("Too few passengers! \n");
  }

  int* passengers_id = calloc(passengers_count, sizeof(int));

  passengers_queue = calloc(passengers_count, sizeof(int));  // queue initialize
  for (int i = 0; i < passengers_count; ++i) {
    passengers_queue[i] = -1;
  }

  pthread_t* passengers_keys = calloc(passengers_count, sizeof(pthread_t));
  for (int i = 0; i < passengers_count; ++i) {
    passengers_id[i] = i;
    add_to_queue(passengers_id[i]);

    if (pthread_create(&passengers_keys[i], NULL, passenger_routine,
                       &passengers_id[i]) != 0) {
      perror("Unable to reate passenger thread.\n");
      exit(3);
    }
  }

  correct_cariage_id = 0;

  int* carriages_id = calloc(carriages_count, sizeof(int));

  carriage_status = calloc(carriages_count, sizeof(int));

  for (int i = 0; i < carriages_count; ++i) {
    carriage_status[i] = EMPTY;
  }

  pthread_t* carriages_keys = calloc(carriages_count, sizeof(pthread_t));
  for (int i = 0; i < carriages_count; ++i) {
    carriages_id[i] = i;
    if (pthread_create(&carriages_keys[i], NULL, carriage_routine,
                       &carriages_id[i]) != 0) {
      perror("Unable to create carriage thread \n");
      exit(3);
    }
  }

  for (int i = 0; i < carriages_count; ++i) {
    pthread_join(carriages_keys[i], NULL);
    printf("Carriage thread: %ld has ended. \n", carriages_keys[i]);
  }

  for (int i = 0; i < passengers_count; ++i) {
    pthread_join(passengers_keys[i], NULL);
    printf("Passenger thread: %ld has ended. \n", passengers_keys[i]);
  }

  pthread_mutex_destroy(&mutex_cart_dock);
  pthread_mutex_destroy(&mutex_passenger);
  pthread_mutex_destroy(&wait_for_release);
  pthread_mutex_destroy(&end);
  pthread_mutex_destroy(&button);

  pthread_cond_destroy(&carriage_order);
  pthread_cond_destroy(&current_passanger_change);
  pthread_cond_destroy(&leaver);
  pthread_cond_destroy(&end_cond);
  pthread_cond_destroy(&button_cond);

  free(passengers_keys);
  free(carriages_keys);
  free(passengers_id);
  free(carriages_keys);
  free(passengers_queue);
}
