#ifndef HOOKLIB_HOOKLIB_H
#define HOOKLIB_HOOKLIB_H

#include <vector>
#include <functional>
#include <iostream>

namespace hooklib {
	using CBReturnEnum = enum {
		CALL_ORIGINAL = 0,
		CANCEL_ORIGINAL = 1,
		STOP = 2
	};

	using EDXType = std::uint32_t;
	using VTable = void**;

	using Class_raw = struct {
		VTable vtable;
	};

	bool PatchMemData(void* pAddr, std::size_t buf_len, void* pNewData, std::size_t data_len);

	template<std::size_t N, typename Cls, typename ReturnType, typename... Args>
	class hook_entry {
		using thisPtrType = std::add_pointer_t<ReturnType __fastcall(Cls*, EDXType, Args...)>;
		using cbPtrType = std::add_pointer_t<CBReturnEnum (Cls*, Args...)>;

	private:
		static inline std::vector <cbPtrType> callbacks;
		static inline thisPtrType thisFunction;

		static ReturnType __fastcall hooked(Cls *This, EDXType EDX, Args... this_args) {
			hooklib::CBReturnEnum action = CALL_ORIGINAL;

			for (const auto &cb : callbacks) {
				auto cb_action = cb(This, this_args...);
				switch(cb_action) {
					case CANCEL_ORIGINAL:
						action = CANCEL_ORIGINAL;
						break;
					case STOP:
						return 0;
					default:
						break;
				}
			}

			if (action == CALL_ORIGINAL) {
				return thisFunction(This, EDX, this_args...);
			}

			return 0;
		}

		static thisPtrType vtableGetFuncAddr(VTable vtable) {
			return reinterpret_cast<thisPtrType>(vtable[N]);
		}

		static void vtableSetFuncAddr(VTable vtable) {
			thisPtrType hkd = &hooked;
			PatchMemData(&vtable[N], sizeof(vtable[N]), &hkd, sizeof(vtable[N]));
		}

		static inline VTable vtableFromClassPtr(Cls *cl) {
			const auto cl_raw = reinterpret_cast<Class_raw*>(cl);
			return cl_raw->vtable;
		}

		static thisPtrType classPtrGetFuncAddr(Cls* cl) {
			const auto vtable = vtableFromClassPtr(cl);
			return vtableGetFuncAddr(vtable);
		}

		static void classPtrSetFuncAddr(Cls* cl) {
			const auto vtable = vtableFromClassPtr(cl);
			return vtableSetFuncAddr(vtable);
		}

	public:
		static void vtableHookFunction(VTable vtable) {
			thisFunction = vtableGetFuncAddr(vtable);
			vtableSetFuncAddr(vtable);
		}

		template<typename SameCls>
		static void classPtrHookFunction(SameCls *cl) {
			static_assert(std::is_same_v<Cls, SameCls>);
			thisFunction = classPtrGetFuncAddr(cl);
			classPtrSetFuncAddr(cl);
		}

		static void registerCallback(CBReturnEnum (*ptrCb)(Cls *, Args...))  {
			callbacks.emplace_back(ptrCb);
		}

		static void clearCallbacks() {
			callbacks.clear();
		}
	};

	template<std::size_t N, typename ForceCls = void, typename Cls, typename ReturnType, typename... Args>
	constexpr auto get(ReturnType (Cls:: *)(Args...)) {
		if constexpr(std::is_same_v<ForceCls, void>) {
			return hook_entry<N, Cls, ReturnType, Args...>();
		} else {
			return hook_entry<N, ForceCls, ReturnType, Args...>();
		}
	}
}

#endif //HOOKLIB_HOOKLIB_H
