// linked_list.c (slide 62)

#define _CRT_SECURE_NO_WARNINGS // Visual Studio에서 scanf 경고 무시
#include <stdio.h>
#include <stdlib.h>     //malloc.h 대신 사용
#include <string.h>

// 1. 학생 정보를 저장할 구조체 정의
typedef struct {
    char name[20];
    int kor;
    int eng;
    int math;
    int total;
    double average;
    int rank;
} Student;

// 2. 연결 리스트의 노드 구조체 정의
typedef struct Node {
    Student data;       // 학생 정보 데이터
    struct Node* next;  // 다음 노드를 가리킬 포인터
} Node;

// 함수 프로토타입 선언
void AddStudent(Node** pp_head, Node** pp_tail, Student new_student);
void LoadFromFile(Node** pp_head, Node** pp_tail);
void SaveToFile(Node* p_head);
void InputStudent(Node** pp_head, Node** pp_tail);
void PrintGrades(Node* p_head);
void FreeList(Node** pp_head);
void CalculateRank(Node* p_head);

int main(void) {
    Node* p_head = NULL; // 리스트의 시작
    Node* p_tail = NULL; // 리스트의 끝
    int choice;

    while (1) {
        printf("\n[Menu]\n");
        printf("1. .dat 파일에서 데이터 읽기\n");
        printf("2. 추가 학생 정보 입력\n");
        printf("3. .dat 파일 저장\n");
        printf("4. 성적 확인 (평균 계산 등)\n");
        printf("5. 종료\n");
        printf("-------------------\n");
        printf("선택(1~5): ");
        
        if (scanf("%d", &choice) != 1) { // 입력 오류 처리
            while (getchar() != '\n'); // 버퍼 비우기
            continue;
        }

        switch (choice) {
        case 1:
            // 기존 리스트를 비우고 새로 불러올지, 추가할지 결정해야 하나 여기선 추가/로드 단순화
            LoadFromFile(&p_head, &p_tail);
            break;
        case 2:
            InputStudent(&p_head, &p_tail);
            break;
        case 3:
            SaveToFile(p_head);
            break;
        case 4:
            PrintGrades(p_head);
            break;
        case 5:
            FreeList(&p_head); // 메모리 해제
            printf("프로그램을 종료합니다.\n");
            return 0;
        default:
            printf("잘못된 선택입니다.\n");
        }
    }
    return 0;
}

// 리스트 끝에 학생 노드를 추가하는 함수 (기존 AddNumber 수정)
void AddStudent(Node** pp_head, Node** pp_tail, Student new_student) {
    Node* new_node = (Node*)malloc(sizeof(Node));
    if (new_node == NULL) {
        printf("메모리 할당 실패\n");
        return;
    }
    new_node->data = new_student;
    new_node->next = NULL;

    if (*pp_head == NULL) {
        *pp_head = new_node;
        *pp_tail = new_node;
    }
    else {
        (*pp_tail)->next = new_node;
        *pp_tail = new_node;
    }
}

// 1. 파일에서 읽기
void LoadFromFile(Node** pp_head, Node** pp_tail) {
    FILE* fp = fopen("students.dat", "rb");
    if (fp == NULL) {
        printf("파일을 열 수 없거나 파일이 존재하지 않습니다.\n");
        return;
    }

    Student temp;
    int count = 0;
    
    // 파일 끝까지 구조체 단위로 읽기
    while (fread(&temp, sizeof(Student), 1, fp) == 1) {
        AddStudent(pp_head, pp_tail, temp);
        count++;
    }

    printf("%d명의 학생 정보를 불러왔습니다.\n", count);
    fclose(fp);
}

// 2. 학생 정보 입력 받기
void InputStudent(Node** pp_head, Node** pp_tail) {
    Student s;
    
    printf("이름: ");
    scanf("%s", s.name);
    printf("국어: ");
    scanf("%d", &s.kor);
    printf("영어: ");
    scanf("%d", &s.eng);
    printf("수학: ");
    scanf("%d", &s.math);

    // 총점과 평균 계산
    s.total = s.kor + s.eng + s.math;
    s.average = s.total / 3.0;
    s.rank = 0; // 등수는 성적 확인 시 계산

    AddStudent(pp_head, pp_tail, s);
    printf("학생 정보가 추가되었습니다.\n");
}

// 3. 파일로 저장하기
void SaveToFile(Node* p_head) {
    FILE* fp = fopen("students.dat", "wb");
    if (fp == NULL) {
        printf("파일 저장 실패\n");
        return;
    }

    Node* p = p_head;
    int count = 0;
    while (p != NULL) {
        // 구조체 데이터를 통째로 쓰기
        fwrite(&(p->data), sizeof(Student), 1, fp);
        p = p->next;
        count++;
    }

    printf("%d명의 학생 정보를 저장했습니다.\n", count);
    fclose(fp);
}

// 등수 계산 함수
void CalculateRank(Node* p_head) {
    Node* i = p_head;
    while (i != NULL) {
        i->data.rank = 1; // 기본 1등으로 초기화
        Node* j = p_head;
        while (j != NULL) {
            // 다른 학생의 총점이 더 높으면 내 등수 +1
            if (j->data.total > i->data.total) {
                i->data.rank++;
            }
            j = j->next;
        }
        i = i->next;
    }
}

// 4. 성적 확인 및 출력
void PrintGrades(Node* p_head) {
    if (p_head == NULL) {
        printf("데이터가 없습니다.\n");
        return;
    }

    // 출력 전 등수 재계산
    CalculateRank(p_head);

    printf("\n%-10s %-6s %-6s %-6s %-6s %-6s %-4s\n", "이름", "국어", "영어", "수학", "총점", "평균", "등수");
    printf("----------------------------------------------------------\n");

    Node* p = p_head;
    while (p != NULL) {
        printf("%-10s %-6d %-6d %-6d %-6d %-6.2f %-4d\n",
            p->data.name,
            p->data.kor, p->data.eng, p->data.math,
            p->data.total, p->data.average, p->data.rank);
        p = p->next;
    }
}

// 5. 메모리 해제 (종료 시)
void FreeList(Node** pp_head) {
    Node* p = *pp_head;
    Node* next_node;

    while (p != NULL) {
        next_node = p->next;
        free(p);
        p = next_node;
    }
    *pp_head = NULL;
}