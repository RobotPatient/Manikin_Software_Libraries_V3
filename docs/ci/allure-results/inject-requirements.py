import xml.etree.ElementTree as ET
import re
import sys
import os

if len(sys.argv) != 2:
    print("Usage: python3 inject_requirements.py <junit-results.xml>")
    sys.exit(1)

xml_file = sys.argv[1]

if not os.path.exists(xml_file):
    print(f"Error: File not found: {xml_file}")
    sys.exit(1)

# Adjust this base URL if your MkDocs is hosted somewhere specific
REQUIREMENT_BASE_URL = "requirements.md#"

tree = ET.parse(xml_file)
root = tree.getroot()

for testcase in root.iter("testcase"):
    name = testcase.get("name", "")

    # Find all [REQ-XX] style tags
    req_tags = re.findall(r"\[REQ-[A-Za-z0-9\-]+\]", name)

    for tag in req_tags:
        req_id = tag.strip("[]")

        # Add Allure label
        label = ET.SubElement(testcase, "label")
        label.set("name", "requirement")
        label.set("value", req_id)

        # Add Allure link (pointing to an anchor within requirements.md)
        link = ET.SubElement(testcase, "link")
        link.set("type", "custom")
        link.set("name", req_id)
        link.set("url", f"{REQUIREMENT_BASE_URL}{req_id}")

tree.write(xml_file, encoding="utf-8", xml_declaration=True)
print(f"✅ Injected requirement labels and links into: {xml_file}")
