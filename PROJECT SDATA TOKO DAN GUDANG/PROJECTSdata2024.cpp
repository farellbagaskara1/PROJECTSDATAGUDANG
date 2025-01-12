#include <iostream>
#include <fstream>
#include <string>
#include <queue>
#include <vector>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <stdexcept>

using namespace std;

// Fungsi untuk menambahkan satu hari ke tanggal
string incrementDate(string currentDate) {
    int year, month, day;
    char separator;

    stringstream ss(currentDate);
    ss >> year >> separator >> month >> separator >> day;

    if (ss.fail() || separator != '-' || year < 1 || month < 1 || month > 12 || day < 1 || day > 31) {
        cerr << "Format tanggal tidak valid. Harus dalam format YYYY-MM-DD.\n";
        return currentDate;
    }

    int daysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    bool isLeapYear = (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
    if (isLeapYear) daysInMonth[1] = 29;

    day++;
    if (day > daysInMonth[month - 1]) {
        day = 1;
        month++;
        if (month > 12) {
            month = 1;
            year++;
        }
    }

    stringstream result;
    result << setw(4) << setfill('0') << year << "-"
           << setw(2) << setfill('0') << month << "-"
           << setw(2) << setfill('0') << day;

    return result.str();
}

struct Barang {
    string idBarang;
    string namaBarang;
    string tanggalMasuk;
};

struct Peserta {
    string nama;
    string nik;
    time_t waktuKedatangan;
};

struct NodeBarang {
    Barang data;
    NodeBarang* next;
};

class CircularLinkedListBarang {
private:
    NodeBarang* head;

public:
    CircularLinkedListBarang() : head(NULL) {}

    void tambahBarang(Barang barang) {
        NodeBarang* newNode = new NodeBarang{barang, NULL};
        if (head == NULL) {
            head = newNode;
            head->next = head;
        } else {
            NodeBarang* temp = head;
            while (temp->next != head) {
                temp = temp->next;
            }
            temp->next = newNode;
            newNode->next = head;
        }
    }

    Barang ambilBarangTerakhir() {
        if (head == NULL) {
            throw runtime_error("Gudang kosong.");
        }

        NodeBarang* temp = head;
        NodeBarang* prev = NULL;
        while (temp->next != head) {
            prev = temp;
            temp = temp->next;
        }

        Barang barangTerakhir = temp->data;

        if (prev == NULL) {
            delete head;
            head = NULL;
        } else {
            prev->next = head;
            delete temp;
        }

        return barangTerakhir;
    }

    void cetakBarang() {
        if (head == NULL) {
            cout << "Gudang kosong." << endl;
            return;
        }

        // Simpan barang dalam vektor untuk mencetak dalam urutan terbalik
        vector<Barang> barangList;
        NodeBarang* temp = head;
        do {
            barangList.push_back(temp->data);
            temp = temp->next;
        } while (temp != head);

        // Cetak barang dalam urutan terbalik
        cout << left << setw(15) << "Kode Item"
             << setw(20) << "Nama Barang"
             << setw(20) << "Tanggal Masuk Gudang" << endl;
        cout << "----------------------------------------------------------" << endl;

        for (int i = barangList.size() - 1; i >= 0; --i) {
            cout << left << setw(15) << barangList[i].idBarang
                 << setw(20) << barangList[i].namaBarang
                 << setw(20) << barangList[i].tanggalMasuk << endl;
        }
    }
};

class Toko {
private:
    CircularLinkedListBarang gudang;
    queue<Peserta> antrianPembeli;
    vector<pair<Peserta, Barang> > logTransaksi;
    string currentTanggalGudang = "2020-09-07";

    string formatWaktu(time_t waktu) {
        char buffer[20];
        strftime(buffer, sizeof(buffer), "%H:%M", localtime(&waktu));
        return string(buffer);
    }

    void tampilkanTabelKTP(const string& nama, const string& nik) {
        cout << "===================================" << endl;
        cout << "| Nama                          | " << setw(20) << left << nama << " |" << endl;
        cout << "| NIK                           | " << setw(20) << left << nik << " |" << endl;
        cout << "===================================" << endl;
    }

    void cetakLogTransaksi() {
        if (logTransaksi.empty()) {
            cout << "Belum ada transaksi sebelumnya." << endl;
            return;
        }

        cout << "\n=== Log Transaksi Sebelumnya ===" << endl;
        cout << "=======================================================" << endl;
        cout << "| KTP    | Waktu Kedatangan | Kode Item Barang |" << endl;
        cout << "=======================================================" << endl;

        for (size_t i = 0; i < logTransaksi.size(); ++i) {
            const Peserta& pembeli = logTransaksi[i].first;
            const Barang& barang = logTransaksi[i].second;
            cout << "| " << setw(7) << pembeli.nik
                 << " | " << setw(17) << formatWaktu(pembeli.waktuKedatangan)
                 << " | " << setw(16) << barang.idBarang << " |" << endl;
        }

        cout << "=======================================================" << endl;
    }

public:
    void adminSignin() {
    const string PASSWORD = "tokofarell77";
    const int MAX_ADMIN = 3;
    string nama, nik, password;

    // Membuka file dalam mode append, tapi tambahkan header jika file baru
    ofstream file("data_ktp.txt", ios::app);
    ifstream checkFile("data_ktp.txt");
    bool isNewFile = checkFile.peek() == ifstream::traits_type::eof();
    checkFile.close();

    if (!file.is_open()) {
        cout << "Gagal membuka file untuk menyimpan data KTP." << endl;
        return;
    }

    if (isNewFile) {
        // Tulis header jika file baru
        file << left << setw(20) << "Nama" << setw(20) << "NIK" << endl;
        file << string(40, '=') << endl;
    }

    cout << "=== Farell Store ===" << endl;
    cout << "Masukkan Password: ";
    cin >> password;

    if (password != PASSWORD) {
        cout << "Password salah. Akses ditolak." << endl;
        return;
    }

    int jumlahAdmin;
    cout << "Berapa orang yang ingin sign-in? (Maksimal " << MAX_ADMIN << "): ";
    cin >> jumlahAdmin;

    if (jumlahAdmin > MAX_ADMIN || jumlahAdmin <= 0) {
        cout << "Jumlah admin tidak valid." << endl;
        return;
    }

    cin.ignore();
    for (int i = 0; i < jumlahAdmin; ++i) {
        cout << "\n==== Admin Sign-In ke-" << (i + 1) << " ====" << endl;
        cout << "Masukkan Nama: ";
        getline(cin, nama);
        cout << "Masukkan NIK: ";
        getline(cin, nik);

        // Tulis ke file dalam format tabel
        file << left << setw(20) << nama << setw(20) << nik << endl;

        cout << "Admin " << nama << " berhasil sign-in." << endl;
    }

    file.close();
    cout << "Data KTP berhasil disimpan ke file data_ktp.txt." << endl;
}


    void tampilkanDataKTP() {
        ifstream file("data_ktp.txt");
        if (!file.is_open()) {
            cout << "Gagal membuka file untuk membaca data KTP." << endl;
            return;
        }

        cout << "\n=== Data KTP ===" << endl;
        cout << left << setw(20) << "Nama" << setw(20) << "NIK" << endl;
        cout << "========================================" << endl;

        string line;
        while (getline(file, line)) {
            cout << line << endl;
        }

        file.close();
    }

    void tambahBarangKeGudang() {
        int jumlahBarang;
        cout << "Masukkan jumlah barang yang ingin ditambahkan: ";
        cin >> jumlahBarang;

        for (int i = 0; i < jumlahBarang; ++i) {
            Barang barang;
            cout << "Masukkan ID Barang: ";
            cin.ignore();
            getline(cin, barang.idBarang);
            cout << "Masukkan Nama Barang: ";
            getline(cin, barang.namaBarang);
            barang.tanggalMasuk = currentTanggalGudang;
            gudang.tambahBarang(barang);
            cout << "Barang berhasil ditambahkan ke gudang." << endl;

            currentTanggalGudang = incrementDate(currentTanggalGudang);
        }
    }

    void tambahPembeliKeAntrian() {
        int jumlahPembeli;
        cout << "Masukkan jumlah pembeli yang ingin ditambahkan: ";
        cin >> jumlahPembeli;

        static time_t waktuSekarang = 9 * 3600; // Jam 09:00
        for (int i = 0; i < jumlahPembeli; ++i) {
            Peserta pembeli;
            cout << "Masukkan Nama Pembeli: ";
            cin.ignore();
            getline(cin, pembeli.nama);
            cout << "Masukkan NIK Pembeli: ";
            getline(cin, pembeli.nik);
            pembeli.waktuKedatangan = waktuSekarang;
            waktuSekarang += 15 * 60; // Tambah 15 menit
            antrianPembeli.push(pembeli);
            cout << "Pembeli berhasil ditambahkan ke antrian." << endl;
        }
    }

    void layaniPembeli() {
        if (antrianPembeli.empty()) {
            cout << "Tidak ada pembeli dalam antrian." << endl;
            return;
        }

        try {
            Barang barangDijual = gudang.ambilBarangTerakhir();
            Peserta pembeli = antrianPembeli.front();
            antrianPembeli.pop();

            logTransaksi.push_back(make_pair(pembeli, barangDijual));

            cout << "\n=== Transaksi Pembelian ===" << endl;
            tampilkanTabelKTP(pembeli.nama, pembeli.nik);
            cout << "Waktu Kedatangan: " << formatWaktu(pembeli.waktuKedatangan) << endl;
            cout << "Kode Barang: " << barangDijual.idBarang << endl;
            cout << "Nama Barang: " << barangDijual.namaBarang << endl;
            cout << "Transaksi selesai!\n" << endl;

            cetakLogTransaksi();

        } catch (const runtime_error& e) {
            cout << e.what() << endl;
        }
    }

    void cetakBarangDiGudang() {
        cout << "=== Daftar Barang di Gudang ===" << endl;
        gudang.cetakBarang();
    }
};

int main() {
    Toko toko;
    toko.adminSignin();
    toko.tampilkanDataKTP();

    int pilihan;
    do {
        cout << "\n=== Menu Toko ===" << endl;
        cout << "1. Tambah Barang ke Gudang" << endl;
        cout << "2. Tambah Pembeli ke Antrian" << endl;
        cout << "3. Layani Pembeli" << endl;
        cout << "4. Cetak Daftar Barang di Gudang" << endl;
        cout << "5. Keluar" << endl;
        cout << "Pilih: ";
        cin >> pilihan;

        switch (pilihan) {
        case 1:
            toko.tambahBarangKeGudang();
            break;
        case 2:
            toko.tambahPembeliKeAntrian();
            break;
        case 3:
            toko.layaniPembeli();
            break;
        case 4:
            toko.cetakBarangDiGudang();
            break;
        case 5:
            cout << "Keluar dari program." << endl;
            break;
        default:
            cout << "Pilihan tidak valid." << endl;
        }
    } while (pilihan != 5);

    return 0;
}

