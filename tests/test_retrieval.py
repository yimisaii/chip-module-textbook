import subprocess
import sys
import tempfile
import unittest
from pathlib import Path


SCRIPT = Path(__file__).resolve().parents[1] / "scripts" / "retrieve.py"


def retrieve(chip, peripheral, sdk, query, library=None):
    command = [sys.executable, "-X", "utf8", str(SCRIPT), "--chip", chip,
               "--peripheral", peripheral, "--sdk", sdk, query]
    if library is not None:
        command += ["--library", str(library)]
    return subprocess.run(command, capture_output=True, encoding="utf-8", check=False)


class RetrievalTest(unittest.TestCase):
    def test_stm32_dma_counts_bytes(self):
        result = retrieve("stm32u575", "dma", "hal1-1.6.3", "64 256 BNDT")
        self.assertEqual(result.returncode, 0, result.stderr)
        self.assertIn("stm32u575/dma/transfer-count.md:", result.stdout)
        self.assertRegex(result.stdout, r"256\s*字节")
        self.assertNotIn("rp2040/", result.stdout)

    def test_rp2040_dma_counts_transfers(self):
        result = retrieve("rp2040", "dma", "pico-sdk-2.2.0", "64 256 transfer_count")
        self.assertEqual(result.returncode, 0, result.stderr)
        self.assertIn("rp2040/dma/transfer-count.md:", result.stdout)
        self.assertRegex(result.stdout, r"64\s*次传输")
        self.assertNotIn("stm32u575/", result.stdout)
        self.assertNotIn("BNDT", result.stdout)

    def test_unmatched_sdk_yields_no_technical_evidence(self):
        result = retrieve("stm32u575", "dma", "hal2", "BNDT")
        self.assertEqual(result.returncode, 1)
        self.assertEqual(result.stdout, "")

    def test_esp32c3_ledc_retrieves_the_pin_and_nominal_waveform(self):
        result = retrieve("esp32c3", "ledc", "esp-idf-5.3.4", "J3-11 250 750")
        self.assertEqual(result.returncode, 0, result.stderr)
        self.assertIn("esp32c3/ledc/board.md:", result.stdout)
        self.assertIn("esp32c3/ledc/timer.md:", result.stdout)
        self.assertIn("J3-11 对应 IO4", result.stdout)
        self.assertIn("250 μs", result.stdout)
        self.assertIn("750 μs", result.stdout)

    def test_esp32c3_ledc_does_not_fall_back_to_another_sdk(self):
        result = retrieve("esp32c3", "ledc", "esp-idf-5.4.0", "GPIO4 250")
        self.assertEqual(result.returncode, 1)
        self.assertEqual(result.stdout, "")

    def test_same_chip_other_peripheral_is_excluded(self):
        result = retrieve("stm32u575", "gpio-exti", "hal1-1.6.3", "BNDT")
        self.assertEqual(result.returncode, 1)
        self.assertEqual(result.stdout, "")

    def test_exact_sdk_wins_with_both_versions_in_same_folder(self):
        with tempfile.TemporaryDirectory() as root:
            scope = Path(root) / "demo" / "dma"
            scope.mkdir(parents=True)
            (scope / "new.md").write_text(
                "sdk: v2\nsource: demo v2\n\n# DMA\n\ntransfer current-api\n", encoding="utf-8")
            (scope / "old.md").write_text(
                "sdk: v1\nsource: demo v1\n\n# DMA\n\ntransfer old-api\n", encoding="utf-8")
            result = retrieve("demo", "dma", "v2", "transfer", root)
        self.assertEqual(result.returncode, 0, result.stderr)
        self.assertIn("current-api", result.stdout)
        self.assertNotIn("old-api", result.stdout)


if __name__ == "__main__":
    unittest.main()
