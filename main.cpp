#include <iostream>
#include <cstdlib>
#include <iomanip>

using namespace std;

void perform_xor(unsigned long long n, unsigned long long p, unsigned int& op, 
                 unsigned long long& mask, unsigned long long msb) {  
    do {
        if ( mask && !(op & msb) ) {    // bring down bits
            op = op << 1;               // dismiss leftmost 0
            if (n & mask)               // and bring down next 0
                op |= 1;                // or 1
            mask = mask >> 1;
        }
        if ( op & msb )     // if MSB is 1
            op = op ^ p;    // perform XOR
    } while (mask);
}

// calculate the FCS of a frame
unsigned int fcs(long long n, unsigned int k, unsigned long long p, unsigned int CRC) {
    unsigned long long bitstream = (unsigned long long) n;  // the data
    unsigned long long mask = 1ULL << ( k - 1 );    // the next bit to bring down
    unsigned long long msb = 1ULL << CRC;           // most significant 1

    // create 1st operand
    unsigned int op = 0;
    for (size_t i = 0; i < CRC + 1; i++) {
        op = op << 1;
        if (bitstream & mask)
            op |= 1;
        mask = mask >> 1;   
    }

    perform_xor(bitstream, p, op, mask, msb);   // perform xor until I run out of bits

    bitstream = bitstream << CRC;  // number + FCS
    mask = 1ULL << (CRC-1);

    perform_xor(bitstream, p, op, mask, msb);   // perform the rest of the xor

    return op;
}

// perform CRC
bool crc(long long n, unsigned int k, unsigned long long p, unsigned int CRC, unsigned int FCS) {
    unsigned long long bitstream = (unsigned long long) n;  // the data
    unsigned long long mask = 1ULL << ( k - 1 );    // the next bit to bring down
    unsigned long long msb = 1ULL << CRC;           // most significant 1

    // create 1st operand
    unsigned int op = 0;
    for (size_t i = 0; i < CRC + 1; i++) {
        op = op << 1;
        if (bitstream & mask)
            op |= 1;
        mask = mask >> 1;   
    }

    perform_xor(bitstream, p, op, mask, msb);   // perform xor until I run out of bits

    bitstream = (bitstream << CRC) | FCS;  // number + FCS
    mask = 1ULL << (CRC-1);

    perform_xor(bitstream, p, op, mask, msb);   // perform the rest of the xor

    if (op)
        return false;   // CRC failed
    else
        return true;    // CRC passed
}


int main() {
    long long n;            // the data to transmit
    unsigned int k = 20;    // length of data
    unsigned long long p;   // polynomial
    unsigned int CRC = 0;   // bits of FCS
    unsigned int FCS;       // the bits that will accompany the data

    cout << "Enter the polynomial: ";
    cin >> p;
    if ( !(p & 1) ) {
        cout << "Invalid polynomial" << endl;
        return 0;
    }

    // convert to decimal
    unsigned long long temp = p;
    p = 0;
    unsigned int exp = 1;
    while (temp) {
        unsigned int bit = temp % 10;
        if ( bit > 1)
            cout << "Invalid polynomial; Enter in binary mode." << endl;
        temp = temp / 10;
        p += bit * exp;
        exp = exp << 1;
        CRC++;
    }
    CRC--;
    
    const unsigned int BER = 1e3;          // Bit Error Rate
    const unsigned long FRAMES = 1000000;
    unsigned long errors = 0;               // Frames transmitted with error(s)
    unsigned long failed = 0;               // Frames where CRC failed
    unsigned long undetected = 0;             // Frames transmitted with error(s) which CRC failed to detect
    srand(time(0));
    for (size_t i = 0; i < FRAMES; i++) {
        
        // build datastream
        n = 0;
        for (size_t j = 0; j < k; j++) {
            n = n << 1;
            n = n | ( rand() % 2 );
        }

        FCS = fcs(n, k, p, CRC);   // calculate the FCS

        // simulate error on transmited data
        bool flag = false;
        unsigned long long mask = 1;
        for (size_t j = 0; j < k; j++) {   // for every bit of the frame
            if ( !(rand() % BER) ) {    // probability 1/BER
                n = n ^ mask;
                flag = true;
            }
            mask = mask << 1;
        }

        // simulate error on FCS
        mask = 1;
        for (size_t j = 0; j < CRC; j++) {
            if ( !(rand() % BER) ) {
                FCS = FCS ^ mask;
                flag = true;
            }
            mask = mask << 1;
        }

        if (flag)       // transmited data have been altered
            errors++;

        if ( crc(n, k, p, CRC, FCS)) {     // CRC passed
            if (flag)                   // data were altered
                undetected++;           // CRC failed to detect the errors
        }
        else                          // CRC failed
            failed++;
    }
    
    cout << endl << FRAMES << " frames transmited" << endl;
    cout << fixed << setprecision(3)
         << "Errors in transmission:\t" << errors << "\t" << 100.0*errors/FRAMES << "% of frames" << endl;
    cout << "CRC detected errors:\t" << failed << "\t" << 100.0*failed/FRAMES << "% of frames, "
         << 100.0*failed/errors << "% of errors" << endl;
    cout << "CRC undetected errors:\t" << undetected << "\t" << 100.0*undetected/errors << "% of errors" << endl << endl;

    return 0;
}