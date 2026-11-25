#!/usr/bin/env bash
set -euo pipefail

BIN="./opcp"
TMP_DIR="$(mktemp -d)"
PASS=0
FAIL=0

cleanup() {
    chmod -R u+rw "$TMP_DIR" 2>/dev/null || true
    rm -rf "$TMP_DIR"
}
trap cleanup EXIT

pass() {
    PASS=$((PASS + 1))
    printf "[PASS] %s\n" "$1"
}

fail() {
    FAIL=$((FAIL + 1))
    printf "[FAIL] %s: %s\n" "$1" "$2" >&2
}

if [[ ! -x "$BIN" ]]; then
    echo "error: $BIN not found or not executable; run 'make' first" >&2
    exit 1
fi

# T1: Empty file (0 bytes)
touch "$TMP_DIR/empty.bin"
if "$BIN" "$TMP_DIR/empty.bin" "$TMP_DIR/empty_out.bin" && [[ ! -s "$TMP_DIR/empty_out.bin" ]]; then
    pass "T1 empty file (0 B)"
else
    fail "T1 empty file (0 B)" "exit code or non-empty output"
fi

# T2: Binary data integrity across buffer sizes (default, 1B, 4K, 1M)
dd if=/dev/urandom of="$TMP_DIR/rand.bin" bs=1024 count=256 status=none
SRC_HASH="$(sha256sum "$TMP_DIR/rand.bin" | awk '{print $1}')"

for spec in "default" "1" "4K" "1M"; do
    out="$TMP_DIR/rand_${spec}.bin"
    if [[ "$spec" == "default" ]]; then
        "$BIN" "$TMP_DIR/rand.bin" "$out"
    else
        "$BIN" -b "$spec" "$TMP_DIR/rand.bin" "$out"
    fi
    dst_hash="$(sha256sum "$out" | awk '{print $1}')"
    if [[ "$SRC_HASH" == "$dst_hash" ]]; then
        pass "T2 binary sha256 match (-b $spec)"
    else
        fail "T2 binary sha256 match (-b $spec)" "checksum mismatch"
    fi
done

# T3: Non-existent source (ENOENT) -> exit 1, stderr non-empty, stdout empty
set +e
stdout_t3="$("$BIN" "$TMP_DIR/does_not_exist.bin" "$TMP_DIR/t3_out.bin" 2>"$TMP_DIR/t3.err")"
rc_t3=$?
set -e
if [[ $rc_t3 -eq 1 && -z "$stdout_t3" && -s "$TMP_DIR/t3.err" ]]; then
    pass "T3 missing source (ENOENT -> exit 1, stderr)"
else
    fail "T3 missing source" "rc=$rc_t3 stdout='$stdout_t3' err_size=$(wc -c < "$TMP_DIR/t3.err")"
fi

# T4: Permission denied (EACCES) on unreadable src and read-only dst
if [[ "$(id -u)" -ne 0 ]]; then
    printf "secret" > "$TMP_DIR/noperm_src.bin"
    chmod 000 "$TMP_DIR/noperm_src.bin"
    set +e
    "$BIN" "$TMP_DIR/noperm_src.bin" "$TMP_DIR/t4a_out.bin" >/dev/null 2>"$TMP_DIR/t4a.err"
    rc_t4a=$?
    set -e
    if [[ $rc_t4a -eq 1 && -s "$TMP_DIR/t4a.err" ]]; then
        pass "T4a unreadable source (EACCES -> exit 1, stderr)"
    else
        fail "T4a unreadable source" "rc=$rc_t4a"
    fi

    printf "locked" > "$TMP_DIR/ro_dst.bin"
    chmod 444 "$TMP_DIR/ro_dst.bin"
    set +e
    "$BIN" "$TMP_DIR/rand.bin" "$TMP_DIR/ro_dst.bin" >/dev/null 2>"$TMP_DIR/t4b.err"
    rc_t4b=$?
    set -e
    if [[ $rc_t4b -eq 1 && -s "$TMP_DIR/t4b.err" ]]; then
        pass "T4b read-only destination (EACCES -> exit 1, stderr)"
    else
        fail "T4b read-only destination" "rc=$rc_t4b"
    fi
else
    printf "[SKIP] T4 permission tests (running as root)\n"
fi

# T5: Invalid -b buffer size arguments
for bad in "0" "abc" "-4K" "10X"; do
    set +e
    "$BIN" -b "$bad" "$TMP_DIR/empty.bin" "$TMP_DIR/t5_out.bin" >/dev/null 2>"$TMP_DIR/t5.err"
    rc_t5=$?
    set -e
    if [[ $rc_t5 -eq 1 && -s "$TMP_DIR/t5.err" ]]; then
        pass "T5 invalid -b '$bad' -> exit 1, stderr"
    else
        fail "T5 invalid -b '$bad'" "rc=$rc_t5"
    fi
done

# T6: Invalid CLI arg count
set +e
"$BIN" "$TMP_DIR/empty.bin" >/dev/null 2>"$TMP_DIR/t6.err"
rc_t6=$?
set -e
if [[ $rc_t6 -eq 1 && -s "$TMP_DIR/t6.err" ]]; then
    pass "T6 invalid arg count -> exit 1, stderr"
else
    fail "T6 invalid arg count" "rc=$rc_t6"
fi

printf "\nSummary: %d passed, %d failed\n" "$PASS" "$FAIL"
[[ $FAIL -eq 0 ]]
