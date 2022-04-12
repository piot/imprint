#include <clog/clog.h>
#include <imprint/slab_allocator.h>

//void* self_, size_t size, const char *sourceFile, int line,
  //  const char *description
static void *imprintSlabAllocatorAlloc(void *self_, size_t size, const char *sourceFile, size_t line,
                                       const char *description) {
  ImprintSlabAllocator* self = (ImprintSlabAllocator*)self_;
  for (size_t i = 0; i < self->capacity; ++i) {
    ImprintSlabCache *cache = &self->caches[i];
    if (size <= cache->structSize) {
      return imprintSlabCacheAlloc(cache, size, sourceFile, line, description);
    }
  }

  CLOG_ERROR("unsupported size");
}

static void imprintSlabAllocatorFree(void *self_, void *ptr, const char *sourceFile, size_t line,
const char *description) {
  ImprintSlabAllocator* self = (ImprintSlabAllocator*)self_;
  for (size_t i = 0; i < self->capacity; ++i) {
    ImprintSlabCache *cache = &self->caches[i];
    bool worked = imprintSlabCacheTryToFree(cache, ptr);
    if (worked) {
      return;
    }
  }

  CLOG_ERROR("illegal free")
}

void imprintSlabAllocatorInit(ImprintSlabAllocator *self,
                              ImprintAllocator *allocator,
                              size_t powerOfTwo, size_t capacity,
                              const char *debug) {
  if (capacity > 4) {
    CLOG_ERROR("not supported")
  }

  size_t arraySize = 128;
  for (size_t i = 0; i < capacity; ++i) {
    imprintSlabCacheInit(&self->caches[i], allocator, powerOfTwo + i, arraySize,
                         debug);
  }
  self->capacity = capacity;

  self->info.allocator.allocDebugFn = imprintSlabAllocatorAlloc;
  self->info.allocator.callocDebugFn = 0;
  self->info.freeDebugFn = imprintSlabAllocatorFree;
}