#include <QtTest/QtTest>
#include <QObject>
#include <QDateTime>
#include <QJsonObject>
#include <QUuid>

#include "../../src/models/clipboard_item.h"

/**
 * @brief Unit tests for ClipboardItem validation and behavior
 * 
 * These tests verify the validation logic, data integrity, and edge cases
 * for the ClipboardItem model class.
 */
class TestClipboardItem : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();

    // Constructor Tests
    void testDefaultConstructor();
    void testTextConstructor_validText();
    void testTextConstructor_emptyText();
    void testTextConstructor_whitespaceOnlyText();
    void testTextConstructor_withCustomTimestamp();
    void testTextConstructor_withInvalidTimestamp();
    void testJsonConstructor_validJson();
    void testJsonConstructor_invalidJson();
    void testCopyConstructor();
    void testAssignmentOperator();

    // Validation Tests
    void testIsValid_validItem();
    void testIsValid_defaultConstructed();
    void testIsValid_missingId();
    void testIsValid_invalidText();
    void testIsValid_invalidTimestamp();
    void testIsValid_missingHash();

    // Text Validation
    void testValidateText_validText();
    void testValidateText_emptyText();
    void testValidateText_whitespaceOnly();
    void testValidateText_mixedContent();
    void testValidateText_specialCharacters();
    void testValidateText_unicode();
    void testValidateText_veryLongText();

    // Preview Generation
    void testGeneratePreview_shortText();
    void testGeneratePreview_longText();
    void testGeneratePreview_emptyText();
    void testGeneratePreview_whitespaceText();
    void testGeneratePreview_multilineText();
    void testGeneratePreview_customMaxLength();
    void testGeneratePreview_exactMaxLength();

    // Hash Generation
    void testGenerateHash_validText();
    void testGenerateHash_emptyText();
    void testGenerateHash_sameTextSameHash();
    void testGenerateHash_differentTextDifferentHash();
    void testGenerateHash_consistency();

    // Pin Management
    void testPin_functionality();
    void testUnpin_functionality();
    void testTogglePin_functionality();
    void testPinState_persistence();

    // JSON Serialization
    void testToJson_validItem();
    void testToJson_defaultItem();
    void testFromJson_validJson();
    void testFromJson_missingRequiredFields();
    void testFromJson_invalidTimestamp();
    void testFromJson_invalidText();
    void testFromJson_partialJson();
    void testFromJson_withDerivedFields();
    void testFromJson_withoutDerivedFields();
    void testJsonRoundTrip();

    // Equality and Comparison
    void testEquality_sameContent();
    void testEquality_differentContent();
    void testEquality_invalidItems();
    void testInequality();
    void testLessThan_ordering();
    void testLessThan_sameTimestamp();

    // Edge Cases
    void testExtremelyLongText();
    void testBinaryContent();
    void testSpecialCharacters();
    void testMemoryConsistency();

    // Data Integrity
    void testIdUniqueness();
    void testTimestampAccuracy();
    void testHashConsistency();
    void testPreviewConsistency();

private:
    // Helper methods
    ClipboardItem createValidItem(const QString& text = "Sample text");
    QJsonObject createValidJson();
    void verifyItemIntegrity(const ClipboardItem& item);
};

void TestClipboardItem::initTestCase()
{
    // Initialize test environment
}

void TestClipboardItem::cleanupTestCase()
{
    // Clean up test environment
}

// Constructor Tests

void TestClipboardItem::testDefaultConstructor()
{
    ClipboardItem item;
    
    QVERIFY(!item.isValid());
    QVERIFY(item.id().isEmpty());
    QVERIFY(item.text().isEmpty());
    QVERIFY(item.preview().isEmpty());
    QVERIFY(!item.timestamp().isValid());
    QVERIFY(!item.pinned());
    QVERIFY(item.hash().isEmpty());
}

void TestClipboardItem::testTextConstructor_validText()
{
    QString testText = "Valid clipboard content";
    QDateTime beforeCreation = QDateTime::currentDateTime();
    
    ClipboardItem item(testText);
    
    QDateTime afterCreation = QDateTime::currentDateTime();
    
    QVERIFY(item.isValid());
    QVERIFY(!item.id().isEmpty());
    QCOMPARE(item.text(), testText);
    QVERIFY(!item.preview().isEmpty());
    QVERIFY(item.timestamp().isValid());
    QVERIFY(item.timestamp() >= beforeCreation);
    QVERIFY(item.timestamp() <= afterCreation);
    QVERIFY(!item.pinned());
    QVERIFY(!item.hash().isEmpty());
}

void TestClipboardItem::testTextConstructor_emptyText()
{
    ClipboardItem item("");
    
    QVERIFY(!item.isValid());
    QVERIFY(item.id().isEmpty());
    QCOMPARE(item.text(), QString(""));
    QVERIFY(item.preview().isEmpty());
    QVERIFY(!item.pinned());
    QVERIFY(item.hash().isEmpty());
}

void TestClipboardItem::testTextConstructor_whitespaceOnlyText()
{
    ClipboardItem item("   \n\t  ");
    
    QVERIFY(!item.isValid());
    QVERIFY(item.id().isEmpty());
    QCOMPARE(item.text(), QString("   \n\t  "));
    QVERIFY(item.preview().isEmpty());
    QVERIFY(!item.pinned());
    QVERIFY(item.hash().isEmpty());
}

void TestClipboardItem::testTextConstructor_withCustomTimestamp()
{
    QString testText = "Test content";
    QDateTime customTime = QDateTime::fromString("2025-01-01T12:00:00", Qt::ISODate);
    
    ClipboardItem item(testText, customTime);
    
    QVERIFY(item.isValid());
    QCOMPARE(item.timestamp(), customTime);
    QCOMPARE(item.text(), testText);
}

void TestClipboardItem::testTextConstructor_withInvalidTimestamp()
{
    QString testText = "Test content";
    QDateTime invalidTime; // default constructed = invalid
    QDateTime beforeCreation = QDateTime::currentDateTime();
    
    ClipboardItem item(testText, invalidTime);
    
    QDateTime afterCreation = QDateTime::currentDateTime();
    
    QVERIFY(item.isValid());
    QVERIFY(item.timestamp().isValid());
    QVERIFY(item.timestamp() >= beforeCreation);
    QVERIFY(item.timestamp() <= afterCreation);
}

void TestClipboardItem::testJsonConstructor_validJson()
{
    QJsonObject json = createValidJson();
    
    ClipboardItem item(json);
    
    QVERIFY(item.isValid());
    verifyItemIntegrity(item);
}

void TestClipboardItem::testJsonConstructor_invalidJson()
{
    QJsonObject json;
    json["invalid"] = "data";
    
    ClipboardItem item(json);
    
    QVERIFY(!item.isValid());
}

void TestClipboardItem::testCopyConstructor()
{
    ClipboardItem original = createValidItem();
    ClipboardItem copy(original);
    
    QCOMPARE(copy.id(), original.id());
    QCOMPARE(copy.text(), original.text());
    QCOMPARE(copy.preview(), original.preview());
    QCOMPARE(copy.timestamp(), original.timestamp());
    QCOMPARE(copy.pinned(), original.pinned());
    QCOMPARE(copy.hash(), original.hash());
    QVERIFY(copy.isValid());
}

void TestClipboardItem::testAssignmentOperator()
{
    ClipboardItem original = createValidItem();
    ClipboardItem assigned;
    
    QVERIFY(!assigned.isValid());
    
    assigned = original;
    
    QCOMPARE(assigned.id(), original.id());
    QCOMPARE(assigned.text(), original.text());
    QCOMPARE(assigned.preview(), original.preview());
    QCOMPARE(assigned.timestamp(), original.timestamp());
    QCOMPARE(assigned.pinned(), original.pinned());
    QCOMPARE(assigned.hash(), original.hash());
    QVERIFY(assigned.isValid());
}

// Validation Tests

void TestClipboardItem::testIsValid_validItem()
{
    ClipboardItem item = createValidItem();
    QVERIFY(item.isValid());
}

void TestClipboardItem::testIsValid_defaultConstructed()
{
    ClipboardItem item;
    QVERIFY(!item.isValid());
}

void TestClipboardItem::testIsValid_missingId()
{
    // This test verifies internal consistency - we can't directly create 
    // an item with missing ID from valid text, but we can test via JSON
    QJsonObject json = createValidJson();
    json.remove("id");
    
    ClipboardItem item(json);
    
    // fromJson should generate an ID if missing, so this should be valid
    QVERIFY(item.isValid());
    QVERIFY(!item.id().isEmpty());
}

void TestClipboardItem::testIsValid_invalidText()
{
    QJsonObject json = createValidJson();
    json["text"] = "";
    
    ClipboardItem item(json);
    
    QVERIFY(!item.isValid());
}

void TestClipboardItem::testIsValid_invalidTimestamp()
{
    QJsonObject json = createValidJson();
    json["timestamp"] = "invalid-date";
    
    ClipboardItem item(json);
    
    QVERIFY(!item.isValid());
}

void TestClipboardItem::testIsValid_missingHash()
{
    // Similar to ID, hash should be generated if missing
    QJsonObject json = createValidJson();
    json.remove("hash");
    
    ClipboardItem item(json);
    
    QVERIFY(item.isValid());
    QVERIFY(!item.hash().isEmpty());
}

// Text Validation Tests

void TestClipboardItem::testValidateText_validText()
{
    QVERIFY(ClipboardItem::validateText("Hello World"));
    QVERIFY(ClipboardItem::validateText("123"));
    QVERIFY(ClipboardItem::validateText("a"));
    QVERIFY(ClipboardItem::validateText("Special chars: !@#$%^&*()"));
}

void TestClipboardItem::testValidateText_emptyText()
{
    QVERIFY(!ClipboardItem::validateText(""));
    QVERIFY(!ClipboardItem::validateText(QString()));
}

void TestClipboardItem::testValidateText_whitespaceOnly()
{
    QVERIFY(!ClipboardItem::validateText(" "));
    QVERIFY(!ClipboardItem::validateText("   "));
    QVERIFY(!ClipboardItem::validateText("\n"));
    QVERIFY(!ClipboardItem::validateText("\t"));
    QVERIFY(!ClipboardItem::validateText("  \n\t  "));
}

void TestClipboardItem::testValidateText_mixedContent()
{
    QVERIFY(ClipboardItem::validateText(" Hello "));
    QVERIFY(ClipboardItem::validateText("\nContent\n"));
    QVERIFY(ClipboardItem::validateText("\tTabbed content"));
}

void TestClipboardItem::testValidateText_specialCharacters()
{
    QVERIFY(ClipboardItem::validateText("Line1\nLine2"));
    QVERIFY(ClipboardItem::validateText("UTF-8: café"));
    QVERIFY(ClipboardItem::validateText("Emoji: 🚀"));
    QVERIFY(ClipboardItem::validateText("Mixed: hello\tworld\ntest"));
}

void TestClipboardItem::testValidateText_unicode()
{
    QVERIFY(ClipboardItem::validateText("Chinese: 你好"));
    QVERIFY(ClipboardItem::validateText("Arabic: مرحبا"));
    QVERIFY(ClipboardItem::validateText("Russian: Привет"));
    QVERIFY(ClipboardItem::validateText("Japanese: こんにちは"));
}

void TestClipboardItem::testValidateText_veryLongText()
{
    QString longText = QString("x").repeated(10000);
    QVERIFY(ClipboardItem::validateText(longText));
    
    QString megaLongText = QString("y").repeated(1000000);
    QVERIFY(ClipboardItem::validateText(megaLongText));
}

// Preview Generation Tests

void TestClipboardItem::testGeneratePreview_shortText()
{
    QString shortText = "Short";
    QString preview = ClipboardItem::generatePreview(shortText);
    
    QCOMPARE(preview, shortText);
}

void TestClipboardItem::testGeneratePreview_longText()
{
    QString longText = QString("x").repeated(150);
    QString preview = ClipboardItem::generatePreview(longText);
    
    QVERIFY(preview.length() <= 100);
    QVERIFY(preview.endsWith("..."));
    QCOMPARE(preview.length(), 100); // 97 chars + "..."
}

void TestClipboardItem::testGeneratePreview_emptyText()
{
    QString preview = ClipboardItem::generatePreview("");
    QVERIFY(preview.isEmpty());
}

void TestClipboardItem::testGeneratePreview_whitespaceText()
{
    QString preview = ClipboardItem::generatePreview("   \n\t   ");
    QVERIFY(preview.isEmpty());
}

void TestClipboardItem::testGeneratePreview_multilineText()
{
    QString multiline = "Line 1\nLine 2\n  Line 3  \nLine 4";
    QString preview = ClipboardItem::generatePreview(multiline);
    
    QCOMPARE(preview, "Line 1 Line 2 Line 3 Line 4");
    QVERIFY(!preview.contains('\n'));
    QVERIFY(!preview.contains('\t'));
}

void TestClipboardItem::testGeneratePreview_customMaxLength()
{
    QString text = "This is a test string for custom length";
    QString preview = ClipboardItem::generatePreview(text, 20);
    
    QVERIFY(preview.length() <= 20);
    QVERIFY(preview.endsWith("..."));
    QCOMPARE(preview.length(), 20); // 17 chars + "..."
}

void TestClipboardItem::testGeneratePreview_exactMaxLength()
{
    QString text = QString("x").repeated(100);
    QString preview = ClipboardItem::generatePreview(text, 100);
    
    QCOMPARE(preview.length(), 100);
    QCOMPARE(preview, text);
    QVERIFY(!preview.endsWith("..."));
}

// Hash Generation Tests

void TestClipboardItem::testGenerateHash_validText()
{
    QString text = "Test content";
    QString hash = ClipboardItem::generateHash(text);
    
    QVERIFY(!hash.isEmpty());
    QVERIFY(hash.length() > 0);
    // SHA256 produces 64 character hex string
    QCOMPARE(hash.length(), 64);
}

void TestClipboardItem::testGenerateHash_emptyText()
{
    QString hash = ClipboardItem::generateHash("");
    QVERIFY(hash.isEmpty());
}

void TestClipboardItem::testGenerateHash_sameTextSameHash()
{
    QString text = "Identical content";
    QString hash1 = ClipboardItem::generateHash(text);
    QString hash2 = ClipboardItem::generateHash(text);
    
    QCOMPARE(hash1, hash2);
    QVERIFY(!hash1.isEmpty());
}

void TestClipboardItem::testGenerateHash_differentTextDifferentHash()
{
    QString text1 = "Content A";
    QString text2 = "Content B";
    QString hash1 = ClipboardItem::generateHash(text1);
    QString hash2 = ClipboardItem::generateHash(text2);
    
    QVERIFY(hash1 != hash2);
    QVERIFY(!hash1.isEmpty());
    QVERIFY(!hash2.isEmpty());
}

void TestClipboardItem::testGenerateHash_consistency()
{
    QString text = "Consistency test content with special chars: !@#$%^&*()";
    
    // Generate hash multiple times
    QStringList hashes;
    for (int i = 0; i < 10; ++i) {
        hashes.append(ClipboardItem::generateHash(text));
    }
    
    // All hashes should be identical
    for (const QString& hash : hashes) {
        QCOMPARE(hash, hashes.first());
    }
}

// Pin Management Tests

void TestClipboardItem::testPin_functionality()
{
    ClipboardItem item = createValidItem();
    
    QVERIFY(!item.pinned());
    
    item.pin();
    QVERIFY(item.pinned());
    
    // Pinning again should keep it pinned
    item.pin();
    QVERIFY(item.pinned());
}

void TestClipboardItem::testUnpin_functionality()
{
    ClipboardItem item = createValidItem();
    item.pin();
    
    QVERIFY(item.pinned());
    
    item.unpin();
    QVERIFY(!item.pinned());
    
    // Unpinning again should keep it unpinned
    item.unpin();
    QVERIFY(!item.pinned());
}

void TestClipboardItem::testTogglePin_functionality()
{
    ClipboardItem item = createValidItem();
    
    QVERIFY(!item.pinned());
    
    item.togglePin();
    QVERIFY(item.pinned());
    
    item.togglePin();
    QVERIFY(!item.pinned());
    
    item.togglePin();
    QVERIFY(item.pinned());
}

void TestClipboardItem::testPinState_persistence()
{
    ClipboardItem item = createValidItem();
    item.pin();
    
    // Test copy constructor preserves pin state
    ClipboardItem copy(item);
    QVERIFY(copy.pinned());
    
    // Test assignment preserves pin state
    ClipboardItem assigned;
    assigned = item;
    QVERIFY(assigned.pinned());
    
    // Test JSON round trip preserves pin state
    QJsonObject json = item.toJson();
    ClipboardItem fromJson(json);
    QVERIFY(fromJson.pinned());
}

// JSON Serialization Tests

void TestClipboardItem::testToJson_validItem()
{
    ClipboardItem item = createValidItem();
    QJsonObject json = item.toJson();
    
    QVERIFY(json.contains("id"));
    QVERIFY(json.contains("text"));
    QVERIFY(json.contains("preview"));
    QVERIFY(json.contains("timestamp"));
    QVERIFY(json.contains("pinned"));
    QVERIFY(json.contains("hash"));
    
    QCOMPARE(json["id"].toString(), item.id());
    QCOMPARE(json["text"].toString(), item.text());
    QCOMPARE(json["preview"].toString(), item.preview());
    QCOMPARE(json["pinned"].toBool(), item.pinned());
    QCOMPARE(json["hash"].toString(), item.hash());
}

void TestClipboardItem::testToJson_defaultItem()
{
    ClipboardItem item; // Default constructed = invalid
    QJsonObject json = item.toJson();
    
    // Should still contain all fields, even if empty/invalid
    QVERIFY(json.contains("id"));
    QVERIFY(json.contains("text"));
    QVERIFY(json.contains("preview"));
    QVERIFY(json.contains("timestamp"));
    QVERIFY(json.contains("pinned"));
    QVERIFY(json.contains("hash"));
}

void TestClipboardItem::testFromJson_validJson()
{
    QJsonObject json = createValidJson();
    ClipboardItem item(json);
    
    QVERIFY(item.isValid());
    QCOMPARE(item.text(), json["text"].toString());
    QCOMPARE(item.pinned(), json["pinned"].toBool());
}

void TestClipboardItem::testFromJson_missingRequiredFields()
{
    // Missing text
    QJsonObject json1;
    json1["timestamp"] = QDateTime::currentDateTime().toString(Qt::ISODate);
    ClipboardItem item1(json1);
    QVERIFY(!item1.isValid());
    
    // Missing timestamp
    QJsonObject json2;
    json2["text"] = "Valid text";
    ClipboardItem item2(json2);
    QVERIFY(!item2.isValid());
}

void TestClipboardItem::testFromJson_invalidTimestamp()
{
    QJsonObject json = createValidJson();
    json["timestamp"] = "not-a-valid-date";
    
    ClipboardItem item(json);
    QVERIFY(!item.isValid());
}

void TestClipboardItem::testFromJson_invalidText()
{
    QJsonObject json = createValidJson();
    json["text"] = "";
    
    ClipboardItem item(json);
    QVERIFY(!item.isValid());
}

void TestClipboardItem::testFromJson_partialJson()
{
    QJsonObject json;
    json["text"] = "Valid content";
    json["timestamp"] = QDateTime::currentDateTime().toString(Qt::ISODate);
    // Missing optional fields: id, preview, hash, pinned
    
    ClipboardItem item(json);
    
    QVERIFY(item.isValid());
    QVERIFY(!item.id().isEmpty()); // Should be generated
    QVERIFY(!item.preview().isEmpty()); // Should be generated
    QVERIFY(!item.hash().isEmpty()); // Should be generated
    QVERIFY(!item.pinned()); // Should default to false
}

void TestClipboardItem::testFromJson_withDerivedFields()
{
    QJsonObject json = createValidJson();
    json["preview"] = "Custom preview";
    json["hash"] = "customhash123";
    
    ClipboardItem item(json);
    
    QVERIFY(item.isValid());
    QCOMPARE(item.preview(), QString("Custom preview"));
    QCOMPARE(item.hash(), QString("customhash123"));
}

void TestClipboardItem::testFromJson_withoutDerivedFields()
{
    QJsonObject json;
    json["text"] = "Content for auto-generation";
    json["timestamp"] = QDateTime::currentDateTime().toString(Qt::ISODate);
    // No preview or hash provided
    
    ClipboardItem item(json);
    
    QVERIFY(item.isValid());
    QVERIFY(!item.preview().isEmpty());
    QVERIFY(!item.hash().isEmpty());
    QCOMPARE(item.preview(), ClipboardItem::generatePreview(json["text"].toString()));
    QCOMPARE(item.hash(), ClipboardItem::generateHash(json["text"].toString()));
}

void TestClipboardItem::testJsonRoundTrip()
{
    ClipboardItem original = createValidItem();
    original.pin();
    
    QJsonObject json = original.toJson();
    ClipboardItem reconstructed(json);
    
    QVERIFY(reconstructed.isValid());
    QCOMPARE(reconstructed.id(), original.id());
    QCOMPARE(reconstructed.text(), original.text());
    QCOMPARE(reconstructed.preview(), original.preview());
    // Timestamps may lose millisecond precision during JSON serialization
    QCOMPARE(reconstructed.timestamp().toString(Qt::ISODate), original.timestamp().toString(Qt::ISODate));
    QCOMPARE(reconstructed.pinned(), original.pinned());
    QCOMPARE(reconstructed.hash(), original.hash());
}

// Equality and Comparison Tests

void TestClipboardItem::testEquality_sameContent()
{
    QString text = "Same content";
    ClipboardItem item1(text);
    ClipboardItem item2(text);
    
    QVERIFY(item1 == item2);
    QVERIFY(!(item1 != item2));
}

void TestClipboardItem::testEquality_differentContent()
{
    ClipboardItem item1("Content A");
    ClipboardItem item2("Content B");
    
    QVERIFY(item1 != item2);
    QVERIFY(!(item1 == item2));
}

void TestClipboardItem::testEquality_invalidItems()
{
    ClipboardItem invalid1;
    ClipboardItem invalid2;
    
    QVERIFY(!(invalid1 == invalid2)); // Invalid items with empty hashes should not be equal
    QVERIFY(invalid1 != invalid2);
}

void TestClipboardItem::testInequality()
{
    ClipboardItem item1("Different");
    ClipboardItem item2("Content");
    
    QVERIFY(item1 != item2);
    QCOMPARE(item1 != item2, !(item1 == item2));
}

void TestClipboardItem::testLessThan_ordering()
{
    QDateTime older = QDateTime::fromString("2025-01-01T10:00:00", Qt::ISODate);
    QDateTime newer = QDateTime::fromString("2025-01-01T11:00:00", Qt::ISODate);
    
    ClipboardItem olderItem("Older", older);
    ClipboardItem newerItem("Newer", newer);
    
    // Newer items are "less" (should come first in sorted order)
    QVERIFY(newerItem < olderItem);
    QVERIFY(!(olderItem < newerItem));
}

void TestClipboardItem::testLessThan_sameTimestamp()
{
    QDateTime time = QDateTime::currentDateTime();
    ClipboardItem item1("Content A", time);
    ClipboardItem item2("Content B", time);
    
    // With same timestamp, neither should be less than the other
    QVERIFY(!(item1 < item2));
    QVERIFY(!(item2 < item1));
}

// Edge Cases

void TestClipboardItem::testExtremelyLongText()
{
    QString hugeText = QString("x").repeated(1000000); // 1MB of text
    
    ClipboardItem item(hugeText);
    
    QVERIFY(item.isValid());
    QCOMPARE(item.text(), hugeText);
    QVERIFY(item.preview().length() <= 100);
    QVERIFY(!item.hash().isEmpty());
}

void TestClipboardItem::testBinaryContent()
{
    // Test with content that includes null bytes and binary data
    QString binaryText;
    for (int i = 0; i < 256; ++i) {
        binaryText.append(QChar(i));
    }
    
    ClipboardItem item(binaryText);
    
    QVERIFY(item.isValid());
    QCOMPARE(item.text(), binaryText);
    QVERIFY(!item.hash().isEmpty());
}

void TestClipboardItem::testSpecialCharacters()
{
    QString specialText = "Special: \x00\x01\x02\xff\n\r\t\"'\\";
    
    ClipboardItem item(specialText);
    
    QVERIFY(item.isValid());
    QCOMPARE(item.text(), specialText);
    
    // Test JSON round trip with special characters
    QJsonObject json = item.toJson();
    ClipboardItem fromJson(json);
    QVERIFY(fromJson.isValid());
    QCOMPARE(fromJson.text(), specialText);
}

void TestClipboardItem::testMemoryConsistency()
{
    // Create many items to test for memory leaks or corruption
    QList<ClipboardItem> items;
    
    for (int i = 0; i < 1000; ++i) {
        QString text = QString("Item %1 content").arg(i);
        items.append(ClipboardItem(text));
    }
    
    // Verify all items are still valid and contain correct data
    for (int i = 0; i < items.size(); ++i) {
        QVERIFY(items[i].isValid());
        QCOMPARE(items[i].text(), QString("Item %1 content").arg(i));
    }
}

// Data Integrity Tests

void TestClipboardItem::testIdUniqueness()
{
    QSet<QString> ids;
    
    // Create many items and verify IDs are unique
    for (int i = 0; i < 100; ++i) {
        ClipboardItem item(QString("Content %1").arg(i));
        QVERIFY(!item.id().isEmpty());
        QVERIFY(!ids.contains(item.id()));
        ids.insert(item.id());
    }
    
    QCOMPARE(ids.size(), 100);
}

void TestClipboardItem::testTimestampAccuracy()
{
    QDateTime before = QDateTime::currentDateTime();
    
    ClipboardItem item("Test content");
    
    QDateTime after = QDateTime::currentDateTime();
    
    QVERIFY(item.timestamp() >= before);
    QVERIFY(item.timestamp() <= after);
    QVERIFY(item.timestamp().isValid());
}

void TestClipboardItem::testHashConsistency()
{
    QString text = "Consistency test content";
    ClipboardItem item1(text);
    ClipboardItem item2(text);
    
    QCOMPARE(item1.hash(), item2.hash());
    QCOMPARE(item1.hash(), ClipboardItem::generateHash(text));
    
    // Verify hash doesn't change after operations
    QString originalHash = item1.hash();
    item1.pin();
    item1.unpin();
    QCOMPARE(item1.hash(), originalHash);
}

void TestClipboardItem::testPreviewConsistency()
{
    QString text = "Preview consistency test content";
    ClipboardItem item(text);
    
    QCOMPARE(item.preview(), ClipboardItem::generatePreview(text));
    
    // Verify preview doesn't change after operations
    QString originalPreview = item.preview();
    item.pin();
    item.unpin();
    QCOMPARE(item.preview(), originalPreview);
}

// Helper Methods

ClipboardItem TestClipboardItem::createValidItem(const QString& text)
{
    return ClipboardItem(text);
}

QJsonObject TestClipboardItem::createValidJson()
{
    QJsonObject json;
    json["id"] = QUuid::createUuid().toString(QUuid::WithoutBraces);
    json["text"] = "Sample clipboard content";
    json["preview"] = "Sample clipboard content";
    json["timestamp"] = QDateTime::currentDateTime().toString(Qt::ISODate);
    json["pinned"] = false;
    json["hash"] = ClipboardItem::generateHash("Sample clipboard content");
    return json;
}

void TestClipboardItem::verifyItemIntegrity(const ClipboardItem& item)
{
    QVERIFY(item.isValid());
    QVERIFY(!item.id().isEmpty());
    QVERIFY(!item.text().isEmpty());
    QVERIFY(!item.preview().isEmpty());
    QVERIFY(item.timestamp().isValid());
    QVERIFY(!item.hash().isEmpty());
    
    // Verify derived fields match content
    QCOMPARE(item.preview(), ClipboardItem::generatePreview(item.text()));
    QCOMPARE(item.hash(), ClipboardItem::generateHash(item.text()));
}

QTEST_MAIN(TestClipboardItem)
#include "test_clipboard_item.moc"