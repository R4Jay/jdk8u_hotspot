/*
 * Copyright (c) 2013, 2015, Red Hat, Inc. and/or its affiliates.
 *
 * This code is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 only, as
 * published by the Free Software Foundation.
 *
 * This code is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * version 2 for more details (a copy is included in the LICENSE file that
 * accompanied this code).
 *
 * You should have received a copy of the GNU General Public License version
 * 2 along with this work; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Please contact Oracle, 500 Oracle Parkway, Redwood Shores, CA 94065 USA
 * or visit www.oracle.com if you need additional information or have any
 * questions.
 *
 */

#ifndef SHARE_VM_GC_SHENANDOAH_SHENANDOAHHEAPREGION_HPP
#define SHARE_VM_GC_SHENANDOAH_SHENANDOAHHEAPREGION_HPP

#include "memory/space.hpp"

class ShenandoahHeapRegion : public ContiguousSpace {
private:
  static Monitor _mem_protect_lock;

private:
  static size_t RegionSizeBytes;
  static size_t RegionSizeShift;

private:
  ShenandoahHeap* _heap;
  size_t _region_number;
  volatile jint _live_data;
  MemRegion reserved;

  bool _humongous_start;
  bool _humongous_continuation;

  bool _recycled;

  HeapWord* _new_top;

  volatile jint _critical_pins;

#ifdef ASSERT
  int _mem_protection_level;
#endif

public:
  ShenandoahHeapRegion(ShenandoahHeap* heap, HeapWord* start, size_t regionSize, size_t index);

  static void setup_heap_region_size(size_t initial_heap_size, size_t max_heap_size);

  inline static size_t region_size_bytes() {
    return ShenandoahHeapRegion::RegionSizeBytes;
  }

  inline static size_t region_size_shift() {
    return ShenandoahHeapRegion::RegionSizeShift;
  }

  // Convert to jint with sanity checking
  inline static jint region_size_bytes_jint() {
    assert (ShenandoahHeapRegion::RegionSizeBytes <= max_jint, "sanity");
    return (jint)ShenandoahHeapRegion::RegionSizeBytes;
  }

  // Convert to jint with sanity checking
  inline static jint region_size_shift_jint() {
    assert (ShenandoahHeapRegion::RegionSizeShift <= max_jint, "sanity");
    return (jint)ShenandoahHeapRegion::RegionSizeShift;
  }

  size_t region_number() const;

  // Roll back the previous allocation of an object with specified size.
  // Returns TRUE when successful, FALSE if not successful or not supported.
  bool rollback_allocation(uint size);

  void clear_live_data();
  void set_live_data(size_t s);
  inline void increase_live_data_words(jint s);

  void set_recently_allocated(bool value);
  bool is_recently_allocated() const;

  bool has_live() const;
  size_t get_live_data_bytes() const;
  size_t get_live_data_words() const;

  void print_on(outputStream* st) const;

  size_t garbage() const;

  void recycle();

  void oop_iterate_skip_unreachable(ExtendedOopClosure* cl, bool skip_unreachable_objects);

  void object_iterate_interruptible(ObjectClosure* blk, bool allow_cancel);

  HeapWord* object_iterate_careful(ObjectClosureCareful* cl);

  HeapWord* block_start_const(const void* p) const;

  // Just before GC we need to fill the current region.
  void fill_region();

  bool in_collection_set() const;

  void set_in_collection_set(bool b);

  void set_humongous_start(bool start);
  void set_humongous_continuation(bool continuation);

  bool is_humongous() const;
  bool is_humongous_start() const;
  bool is_humongous_continuation() const;

#ifdef ASSERT
  void memProtectionOn();
  void memProtectionOff();
#endif

  // The following are for humongous regions.  We need to save the
  markOop saved_mark_word;
  void save_mark_word(oop obj) {saved_mark_word = obj->mark();}
  markOop mark_word() {return saved_mark_word;}

  void set_new_top(HeapWord* new_top) { _new_top = new_top; }
  HeapWord* new_top() const { return _new_top; }

  void pin();
  void unpin();

  bool is_pinned();

};

#endif // SHARE_VM_GC_SHENANDOAH_SHENANDOAHHEAPREGION_HPP
