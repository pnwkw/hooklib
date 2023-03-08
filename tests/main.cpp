#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include <hooklib/hooklib.h>

constexpr int VTABLE0_C2_RETURN = 20;
constexpr int VTABLE2_C2_RETURN = 22;
constexpr int VTABLE2_C3_RETURN = 32;

class C1
{
public:
	virtual int vtable0(int, int) = 0;
	virtual void vtable1() = 0;
};

class C2 : public C1
{
public:
	int vtable0(int, int) override { return VTABLE0_C2_RETURN; };
	void vtable1() override{};
	virtual int vtable2(int, int) { return VTABLE2_C2_RETURN; };
	virtual void vtable3(int, int, int){};
};

class C3 : public C2
{
public:
	void vtable1() override{};
	int vtable2(int, int) override { return VTABLE2_C3_RETURN; };
	void vtable3(int, int, int) override{};
};

int invoke(C3 *c)
{
	return c->vtable0(0, 0);
}

static std::uintptr_t LAST_CB_CALLED = 0;

hooklib::CBReturnEnum cb0_c3(C3 *, int, int)
{
	LAST_CB_CALLED = reinterpret_cast<std::uintptr_t>(&cb0_c3);
	return hooklib::CALL_ORIGINAL;
}

TEST_CASE("basic hooking")
{
	C3 c3;
	auto hc3 = hooklib::get<0, C3>(&C3::vtable0);
	// hc3.clearCallbacks();
	hc3.classPtrHookFunction(&c3);
	hc3.registerCallback(cb0_c3);

	CHECK(invoke(&c3) == VTABLE0_C2_RETURN);
	CHECK(LAST_CB_CALLED == reinterpret_cast<std::uintptr_t>(&cb0_c3));
}

hooklib::CBReturnEnum cb0_c3_1(C3 *, int, int)
{
	LAST_CB_CALLED = reinterpret_cast<std::uintptr_t>(&cb0_c3_1);
	return hooklib::CANCEL_ORIGINAL;
}

TEST_CASE("original call cancellation")
{
	C3 c3;
	auto hc3 = hooklib::get<0, C3>(&C3::vtable0);
	// hc3.clearCallbacks();
	hc3.classPtrHookFunction(&c3);
	hc3.registerCallback(cb0_c3_1);

	CHECK(invoke(&c3) == 0);
	CHECK(LAST_CB_CALLED == reinterpret_cast<std::uintptr_t>(&cb0_c3_1));
}

hooklib::CBReturnEnum cb0_c3_2(C3 *, int, int)
{
	LAST_CB_CALLED = reinterpret_cast<std::uintptr_t>(&cb0_c3_2);
	return hooklib::STOP;
}

TEST_CASE("immediate stop of callback chain")
{
	C3 c3;
	auto hc3 = hooklib::get<0, C3>(&C3::vtable0);
	// hc3.clearCallbacks();
	hc3.classPtrHookFunction(&c3);
	hc3.registerCallback(cb0_c3_2);
	hc3.registerCallback(cb0_c3_1);

	CHECK(invoke(&c3) == 0);
	CHECK(LAST_CB_CALLED == reinterpret_cast<std::uintptr_t>(&cb0_c3_2));
}
