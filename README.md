# CRC Implementation and Evaluation

---

## 🎯 Objective

Implement a **Cyclic Redundancy Check (CRC)** error-detection system.  
Build a program (in a programming language of your choice) that:

1. **Generates random binary messages** of length **k** bits at the transmitter (each bit is i.i.d. with `P(0)=P(1)=0.5`).
2. **Computes the CRC (FCS)** for each message using a **user-provided binary generator polynomial `P`**.
3. **Transmits** the message + CRC over a **binary symmetric channel (BSC)** with **Bit Error Rate (BER)** and **receives** the possibly **corrupted** block at the receiver.
4. **Checks the CRC** at the receiver and flags errors accordingly.

---

## 📦 Required Functionality

- **Random message generator** for `k`-bit blocks.  
- **CRC encoder**:
  - Append `r` zero bits (where `r = degree(P)`), perform modulo-2 division by `P`, and append the remainder (FCS).
- **Channel model (BSC)**:
  - Flip each transmitted bit independently with probability **BER**.
- **CRC checker**:
  - At the receiver, perform the same modulo-2 division on the received block (message + FCS).  
  - **Syndrome = 0** ⇒ *no error detected*; otherwise ⇒ *error detected*.

---

## 🔢 Special Case to Evaluate

For:
- `k = 20`  
- `P = 110101` (degree `r = 5`, i.e., \( P(x)=x^5 + x^4 + x^2 + 1 \))  
- `BER = 10^{-3}`

Estimate (via Monte Carlo simulation) the following percentages:

1. **Percentage of messages that arrive with errors** (in either the data block or the CRC) at the receiver.  
2. **Percentage of messages flagged as erroneous by the CRC** (detected errors).  
3. **Percentage of messages that arrive with errors but are **not detected** by the CRC** (undetected errors / CRC failures).

> 🔎 *Note*: (3) = (1) − (2), when all quantities are measured over the same sample size.

---

## 🧠 CRC Essentials (Quick Reference)

- **FCS length**: `r = degree(P)`.  
- **Encoding**:
  1. Let the data block be `M` (k bits).  
  2. Compute `R = (M · x^r) mod P(x)` using modulo-2 division.  
  3. Transmit `T = M || R` (concatenation).
- **Checking**:
  - Compute `T mod P(x)` at the receiver.  
  - If the remainder is **0**, CRC **does not** detect an error; otherwise it **does**.

---

## 🧪 Suggested Evaluation Protocol

- Run a large number **N** of trials (e.g., `N ≥ 10^5`) for stable percentages.  
- For each trial:
  1. Generate a random `k`-bit message `M`.  
  2. Compute FCS `R` using `P`.  
  3. Transmit `T = M || R` through BSC(BER).  
  4. Record:
     - `error_present` = whether any bit in `T` flipped.
     - `crc_detected` = whether receiver’s remainder ≠ 0.
- Aggregate:
  - % with errors = `count(error_present)/N`.  
  - % detected by CRC = `count(crc_detected)/N`.  
  - % undetected errors = `count(error_present AND NOT crc_detected)/N`.

> 💡 Use a fixed **random seed** to make results reproducible, and report **confidence intervals** (e.g., normal approximation) if needed.

---

## ⚙️ Configurable Parameters

- `k` (message length), default `20`  
- `P` (binary polynomial string), default `110101`  
- `BER`, default `1e-3`  
- `N` (number of trials), e.g., `100000`  
- `seed` (RNG seed), optional
