#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_ADAYLAR 20
#define MAX_ISIM    50

/* Merhabalar hocam, umarim iyisinizdir. Iste kodu bu sekilde yazdim, kolay gelsin :) */
/* Tum aday bilgilerini tek yerde topluyoruz */
typedef struct {
    char isimler[MAX_ADAYLAR][MAX_ISIM];
    int  oylar[MAX_ADAYLAR];
    int  adaySayisi;
} Adaylar;

/* Secimle ilgili bilgiler burada tutuluyor */
typedef struct {
    int *oyKullandi;   /* Her secmenin oy kullanip kullanmadigini tutar */
    int  secmenSayisi;
    int  kullanilanOy;
    int  idHatasi;     /* Kayitli olmayan ID sayisi */
    int  gecersizAday; /* Gecersiz aday numarasi sayisi */
    int  tekrarID;     /* Ayni ID ile ikinci kez oy deneme sayisi */
    int  bitti;        /* Secimin bitip bitmedigini gosterir */
} Secim;

/* Fonksiyon prototipleri */
void adaylariGir(Adaylar *a);
void oyVer(Adaylar *a, Secim *s);
void araSonuc(const Adaylar *a, const Secim *s);
int  kazananBul(const Adaylar *a);
void yuzdeHesapla(const Adaylar *a, double yuzde[]);
void sirala(const Adaylar *a, int sira[]);
void sonuclariGoster(const Adaylar *a, const Secim *s);
int  menuOku(void);

int main(void) {
    srand((unsigned int)time(NULL));

    Adaylar a;
    Secim   s;

    /* Baslangicta yapilari sifirliyoruz */
    memset(&a, 0, sizeof(a));
    memset(&s, 0, sizeof(s));

    printf("===== MINI OYLAMA SISTEMI =====\n\n");

    /* Aday sayisi girisi ve dogrulama */
    printf("Aday sayisini girin (1-%d): ", MAX_ADAYLAR);

    if (scanf("%d", &a.adaySayisi) != 1 ||
        a.adaySayisi < 1 || a.adaySayisi > MAX_ADAYLAR) {

        printf("Gecersiz aday sayisi!\n");
        return 1;
    }

    /* Secmen sayisi girisi ve dogrulama */
    printf("Secmen sayisini girin: ");

    if (scanf("%d", &s.secmenSayisi) != 1 ||
        s.secmenSayisi < 1) {

        printf("Gecersiz secmen sayisi!\n");
        return 1;
    }

    /* Her secmen icin oy kullanma durumu dinamik olarak ayrilir */
    s.oyKullandi = (int *)calloc(s.secmenSayisi + 1, sizeof(int));

    if (s.oyKullandi == NULL) {
        printf("Bellek ayirilamadi!\n");
        return 1;
    }

    /* Aday isimleri girilir; bu islem menuden sonra degistirilemez */
    adaylariGir(&a);

    printf("\nSecim baslayabilir!\n");

    int menu;

    do {
        /* Secim bittiyse kullaniciya bilgi vermemiz gerekecek */
        if (s.bitti) {
            printf("\n[Secim bitti. Sonuclari gormek icin 3'u secin.]\n");
        }

        printf("\n===== MENU =====\n");
        printf("1- Oy Kullan\n");
        printf("2- Ara Sonuclari Goster\n");
        printf("3- Secimi Bitir ve Sonuclari Acikla\n");
        printf("4- Cikis\n");
        printf("Seciminiz: ");

        menu = menuOku();

        switch (menu) {

            case 1:
                if (s.bitti) {
                    printf("Secim zaten sona erdi!\n");
                } else {
                    oyVer(&a, &s);
                }
                break;

            case 2:
                araSonuc(&a, &s);
                break;

            case 3:
                s.bitti = 1;
                sonuclariGoster(&a, &s);
                break;

            case 4:
                printf("Cikiliyor...\n");
                break;

            default:
                printf("Gecersiz secim, tekrar deneyin.\n");
        }

    } while (menu != 4);

    /* Dinamik bellek serbest birakiliyor */
    free(s.oyKullandi);

    return 0;
}

/* Menuden sayi okur; harf girilirse buffer temizlenir */
int menuOku(void) {

    int deger;

    if (scanf("%d", &deger) != 1) {

        int c;
        while ((c = getchar()) != '\n' && c != EOF);

        return -1;
    }

    return deger;
}

/* Aday isimlerini kullanicidan alir */
void adaylariGir(Adaylar *a) {

    printf("\nAday isimlerini girin:\n");

    for (int i = 0; i < a->adaySayisi; i++) {

        printf("%d. Aday: ", i + 1);

        /* Buffer tasmasini onlemek icin maksimum 49 karakter aliyoruz */
        scanf("%49s", a->isimler[i]);

        a->oylar[i] = 0;
    }
}

/* Rastgele ID ve aday numarasi uretarak oy kullanma islemini gerceklestirir */
void oyVer(Adaylar *a, Secim *s) {

    /* Tum oylar kullanildiysa secimi bitir */
    if (s->kullanilanOy >= s->secmenSayisi) {

        printf("Tum oylar zaten kullanildi!\n");
        s->bitti = 1;
        return;
    }

    /*
     * Gecersiz ID senaryosunu test edebilmek icin ID araligini
     * secmen sayisinin %30 ustune kadar genisletiyoruz.
     */
    int maxID = (int)(s->secmenSayisi * 1.3);

    if (maxID <= s->secmenSayisi)
        maxID = s->secmenSayisi + 3;

    int id     = (rand() % maxID) + 1;
    int adayNo = (rand() % a->adaySayisi) + 1; /* DUZELTME: 1 ile adaySayisi arasinda uretilir */

    printf("\nID: %d | Aday No: %d\n", id, adayNo);

    /* Kayitli secmen kontrolu */
    if (id > s->secmenSayisi) {

        printf("Ogrenci kayitli degil!\n");
        s->idHatasi++;
        return;
    }

    /* Gecerli aday kontrolu — artik gereksiz ama guvenlik icin birakildi */
    if (adayNo < 1 || adayNo > a->adaySayisi) {

        printf("Gecersiz oy!\n");
        s->gecersizAday++;
        return;
    }

    /* Daha once oy kullanmis mi diye kontrol kismi */
    if (s->oyKullandi[id] == 1) {

        printf("Bu ogrenci zaten oy kullanmis!\n");
        s->tekrarID++;
        return;
    }

    /* Agirlik hesaplama: ID degerine gore 3, 2 veya 1 */
    int agirlik;

    double esik1 = 0.2 * s->secmenSayisi;
    double esik2 = 0.5 * s->secmenSayisi;

    if ((double)id < esik1) {

        agirlik = 3; /* ID < 0.2*N */

    } else if ((double)id <= esik2) {

        agirlik = 2; /* 0.2*N <= ID <= 0.5*N */

    } else {

        agirlik = 1; /* ID > 0.5*N */
    }

    /* Oyu kaydet */
    s->oyKullandi[id] = 1;
    a->oylar[adayNo - 1] += agirlik;
    s->kullanilanOy++;

    printf("Oy kaydedildi! (%s icin, agirlik: x%d)\n",
           a->isimler[adayNo - 1], agirlik);

    /* Tum oylar kullanildiysa secimi otomatik bitir */
    if (s->kullanilanOy >= s->secmenSayisi) {

        printf("\nTum oylar kullanildi! Secim sonlandiriliyor...\n");
        s->bitti = 1;
    }
}

/* Anlık oy durumunu ve reddedilen oy istatistiklerini gosterir */
void araSonuc(const Adaylar *a, const Secim *s) {

    printf("\n--- ARA SONUCLAR ---\n");
    printf("Kullanilan oy: %d / %d\n\n", s->kullanilanOy, s->secmenSayisi);

    for (int i = 0; i < a->adaySayisi; i++) {

        printf("  %-20s : %d puan\n", a->isimler[i], a->oylar[i]);
    }

    printf("\nReddedilen oylar:\n");
    printf("  ID hatasi    : %d\n", s->idHatasi);
    printf("  Gecersiz aday: %d\n", s->gecersizAday);
    printf("  Tekrar oy    : %d\n", s->tekrarID);
}

/*
 * En yuksek oyu alan adayin indeksini dondurur.
 * Beraberlik varsa -1 dondurur (ikinci tur gerekli).
 *
 * DUZELTME: maks 0 ile baslatildi.
 * Eski haliyle maks = -1 idi; hic oy kullanilmamissa
 * 0 > -1 oldugu icin ilk aday yanlis sekilde kazanan seciliyordu
 * ve beraberlik kontrolu devreye girmiyordu.
 */
int kazananBul(const Adaylar *a) {

    int maks  = 0; /* DUZELTME: -1 yerine 0 ile baslat */
    int index = -1;
    int esit  = 0;

    for (int i = 0; i < a->adaySayisi; i++) {

        if (a->oylar[i] > maks) {

            maks  = a->oylar[i];
            index = i;
            esit  = 0;

        } else if (a->oylar[i] == maks && maks > 0) {

            /* Ayni en yuksek puandan birden fazla aday varsa beraberlik */
            esit = 1;
        }
    }

    return (esit || index == -1) ? -1 : index;
}

/* Her adayin oy yuzdesini hesaplar */
void yuzdeHesapla(const Adaylar *a, double yuzde[]) {

    int toplam = 0;

    for (int i = 0; i < a->adaySayisi; i++) {
        toplam += a->oylar[i];
    }

    for (int i = 0; i < a->adaySayisi; i++) {

        /* Toplam sifirsa sifira bolme hatasini onle */
        yuzde[i] = (toplam > 0)
                   ? ((double)a->oylar[i] / toplam) * 100.0
                   : 0.0;
    }
}

/* Adaylari oy sayisina gore azalan sirada siralar (bubble sort) */
void sirala(const Adaylar *a, int sira[]) {

    /* Once indeks dizisini olustur */
    for (int i = 0; i < a->adaySayisi; i++) {
        sira[i] = i;
    }

    /* Bubble sort: en yuksek oy basa gelir */
    for (int i = 0; i < a->adaySayisi - 1; i++) {

        for (int j = 0; j < a->adaySayisi - 1 - i; j++) {

            if (a->oylar[sira[j]] < a->oylar[sira[j + 1]]) {

                int tmp   = sira[j];
                sira[j]   = sira[j + 1];
                sira[j + 1] = tmp;
            }
        }
    }
}

/* Final sonuclarini siralı sekilde ekrana yazdirir */
void sonuclariGoster(const Adaylar *a, const Secim *s) {

    printf("\n========== FINAL SONUCLAR ==========\n");

    double yuzde[MAX_ADAYLAR];
    yuzdeHesapla(a, yuzde);

    int sira[MAX_ADAYLAR];
    sirala(a, sira);

    printf("\n%-4s  %-20s  %-6s  %s\n", "Sira", "Aday", "Puan", "Yuzde");
    printf("--------------------------------------------\n");

    for (int i = 0; i < a->adaySayisi; i++) {

        printf("%-4d  %-20s  %-6d  %.2f%%\n",
               i + 1,
               a->isimler[sira[i]],
               a->oylar[sira[i]],
               yuzde[sira[i]]);
    }

    printf("\nIstatistikler:\n");
    printf("  Gecerli oy   : %d\n", s->kullanilanOy);
    printf("  ID hatasi    : %d\n", s->idHatasi);
    printf("  Gecersiz aday: %d\n", s->gecersizAday);
    printf("  Tekrar oy    : %d\n", s->tekrarID);

    int kazanan = kazananBul(a);

    printf("\n");

    if (kazanan == -1) {

        printf("Kazanan belirlenemedi. Ikinci tur gerekli.\n");

    } else {

        printf("KAZANAN: %s (%d puan)\n",
               a->isimler[kazanan],
               a->oylar[kazanan]);
    }

    printf("=====================================\n");

    /* Hayirli gunler dilerim hocam. */
}