# [ADR-XXXX]: [Decision Title]

* **Status**: [Proposed | Accepted | Rejected | Superseded by [ADR-YYYY](ADR-YYYY.md)]
* **Date**: YYYY-MM-DD
* **Author**: [Your Name / Role]
* **Deciders**: [List of roles/names involved in the decision]

---

## 1. Context & Problem Statement

Describe the context of the problem we are trying to solve. What are the constraints, requirements, and user stories? What issues or technical debt are we addressing?

---

## 2. Decision Drivers

List the architectural objectives or factors that influence this decision:
* Objective 1 (e.g. Unit testability)
* Objective 2 (e.g. Performance / zero heap allocations)
* Objective 3 (e.g. Clean separation of concerns)

---

## 3. Considered Options

Detail the potential design options that were investigated:
1. **Option 1**: [Description of design pattern, pros, and cons]
2. **Option 2**: [Description of design pattern, pros, and cons]
3. **Option 3**: [Description of design pattern, pros, and cons]

---

## 4. Proposed Decision & Rationale

State the selected option and explain why it was chosen. How does it resolve the problem statement and satisfy the decision drivers?

---

## 5. Consequences & Implications

Describe what happens as a result of accepting this decision:
* **Positive Impact**: What becomes easier or more performant?
* **Negative Impact**: What trade-offs do we accept? (e.g. boilerplate overhead)
* **Architectural Shifts**: What new rules or boundaries are established?

---

## 6. Compliance Checklist

Define the rules that reviewers must enforce to verify this decision in code:
- [ ] Rule 1: [e.g. Class X must not include header Y]
- [ ] Rule 2: [e.g. Allocations are prohibited in hot-loop updates]

---

## Related Documents

- [coding_standards.md](../coding_standards.md)
- [reviewing-guidelines.md](../reviewing-guidelines.md)
- [testing_strategy.md](../testing_strategy.md)

